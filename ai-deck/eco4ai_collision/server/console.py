#
#     ||          ____  _ __
#  +------+      / __ )(_) /_______________ _____  ___
#  | 0xBC |     / __  / / __/ ___/ ___/ __ `/_  / / _ \
#  +------+    / /_/ / / /_/ /__/ /  / /_/ / / /_/  __/
#   ||  ||    /_____/_/\__/\___/_/   \__,_/ /___/\___/
#
#  Copyright (C) 2021 Bitcraze AB
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.
import time

import cflib.crtp  # noqa
from cflib.crazyflie import Crazyflie
from cflib.crazyflie.syncCrazyflie import SyncCrazyflie
from cflib.utils import uri_helper
import subprocess
from shlex import split
import threading


# def signal_handler(signal, frame):
#     print("\nprogram exiting gracefully")
#     sys.exit(0)
# signal.signal(signal.SIGINT, signal_handler)

# Reads the CFLIB_URI environment variable for URI or uses default
uri = uri_helper.uri_from_env(default='radio://0/80/2M/E7E7E7E705')
cf_ip = "192.168.4.1"
# cf_ip = "8.8.8.8"

str_line = ""

wifi_th_running = False


def scan_wifi_interface():
    global wifi_th_running
    while wifi_th_running:
        response = subprocess.run(split(f"sudo iw dev wlo1 scan freq 2412"), stdout=subprocess.PIPE)
        # print(response)
        time.sleep(0.2)

def get_wifi_info():
    scan_wifi_interface()
    response = subprocess.run(split(f"iwlist wlo1 scan"), stdout=subprocess.PIPE)
    quality = str(response.stdout).split("\\n")[4].split("=")[1].split()[0].split("/")[0]
    try:
        response = subprocess.run(split(f"iw dev wlo1 link"), stdout=subprocess.PIPE)
        rx = str(response.stdout).split("\\n\\t")[6]
        mcs = rx.split()[5].strip()
    except IndexError:
        mcs = -1

    response = subprocess.run(split(f"sudo iw dev wlo1 scan freq 2412"), stdout=subprocess.PIPE)
    db = str(response.stdout).split("cf2")[0].split("\\n\\t")[6].split()[1]
    return quality, mcs, db


def console_callback(text: str):
    '''A callback to run when we get console text from Crazyflie'''
    
    global str_line

    # text = text.replace("CPX: GAP8: ", "")
    str_line += text
    # print(text, end='')
    if text[-1] == "\n":
        if str_line.find("DATA:") == -1:
            str_line = ""
            return
        str_line.replace("DATA:", "")
        
        str_line = str_line.split("GAP8: ")[1].replace("\n", "")
        # print(str_line)
        
        # response = subprocess.run(split(f"ping -c 1 -W 5000 {cf_ip}"), stdout=subprocess.PIPE)
        # ping = str(response.stdout).split()[12].split("=")[1]
        # response = subprocess.run(split(f"iw dev wlo1 link"), stdout=subprocess.PIPE)
        # rx = str(response.stdout).split("\\n\\t")[6]
        # bitrate = rx.split()[2]
        # mcs = rx.split()[5]
        quality, mcs, db = get_wifi_info()
        # bitrate, mcs = get_wifi_info()

        # print(rx)
        # bitrate = str(response.stdout).split("rx bitrate: ")[1].split()[0] # GET MBit/s
        # mcs = str(response.stdout).split("rx bitrate: ")[1].split()[3] # GET MBit/s

        str_line += f",{db}"
        str_line += f",{mcs}"
        str_line += f",{quality}"
        print(str_line)
        with open("aled.csv", 'a') as fichier:
            fichier.write(str_line+"\n")

        str_line = ""



if __name__ == '__main__':
    # Initialize the low-level drivers
    cflib.crtp.init_drivers()

    # scan_wifi_th = threading.Thread(target=scan_wifi_interface)
    # scan_wifi_th.start()

    header="cp,front_time,end_time,ans_time,total_time,db,MCS,quality/70"
    with open("aled.csv", 'w') as fichier:
        fichier.write(header + '\n')

    # Create Crazyflie object, with cache to avoid re-reading param and log TOC
    cf = Crazyflie(rw_cache='./cache')

    # Add a callback to whenever we receive 'console' text from the Crazyflie
    # This is the output from DEBUG_PRINT calls in the firmware.
    #
    # This could also be a Python lambda, something like:
    #   cf.console.receivedChar.add_callback(lambda text: logger.info(text))
    cf.console.receivedChar.add_callback(console_callback)

    # This will connect the Crazyflie with the URI specified above.
    # You might have to restart your Crazyflie in order to get output
    # from console, since not much is written during regular uptime.

    while True:
        try:
            with SyncCrazyflie(uri, cf=cf) as scf:
                print('[host] Connected, use ctrl-c to quit.')

                while cf.state != 0:
                    time.sleep(1)
        except KeyboardInterrupt:
            # wifi_th_running = False
            # scan_wifi_th.join()
            break
        except:
            print("Waiting connection...")