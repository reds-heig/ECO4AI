#!/usr/bin/env python
# -*- coding: utf-8 -*- 
import cv2
import numpy as np
from tabulate import tabulate
import socket, struct

from cflib.cpx.transports import SocketTransport
from cflib.cpx import CPXPacket, CPXTarget, CPXFunction

import time
from torchvision import transforms
import torch
# TritonClient, for backlane inferce
import tritonclient.http as httpclient
from tritonclient.utils import InferenceServerException

import argparse

import onnx
import onnxruntime as ort
import os


WIFI_CTRL_STATUS_CLIENT_CONNECTED = 0x32
WIFI_CTRL_DATA_TRANSFER           = 0x33

parser = argparse.ArgumentParser(description='Connect to AI-deck JPEG streamer example')
parser.add_argument("-n",  default="192.168.4.1", metavar="ip", help="AI-deck IP")
args = parser.parse_args()

deck_port = 5000
deck_ip = args.n


def receive_data(st):

    # read header 
    p = st.readPacket()
    imgHeader = p.data
    width, height, channel, cp = struct.unpack('<IIIB', imgHeader)

    print("cp: ", cp)
    print("width: ", width)
    print("height: ", height)
    print("channel: ", channel)

    size = width * height * channel
    # size = 1000 # only for test
    tensor_data = bytearray()

    while len(tensor_data) < size:
        p = st.readPacket()
        tensor_data.extend(p.data)

    tensor_l = list(tensor_data)

    print("tensor_data size: ", len(tensor_l))
    print(tensor_l[:10])

    return width, height, channel, cp, tensor_l

def dequantize(data):
    # todo: dequantize
    return data

def end_inference(data, cp):
    # todo: do end inference
    return 0

def main():    

    st = SocketTransport(deck_ip, 5000)
    cpt = 0
    try:
        while True:

            width, height, channel, cp, data = receive_data(st)
            data = dequantize(data)
            out = end_inference(data, cp)

            cpx_data = struct.pack('<Bb', WIFI_CTRL_DATA_TRANSFER, out)
            p = CPXPacket(CPXFunction.WIFI_CTRL, CPXTarget.GAP8, CPXTarget.HOST, cpx_data)
            st.writePacket(p)

            cpt += 1
            cv2.waitKey(1)
    finally:
        print("EXIT")
        cpx_data = struct.pack('<Bb', WIFI_CTRL_STATUS_CLIENT_CONNECTED, 0)
        p = CPXPacket(CPXFunction.WIFI_CTRL, CPXTarget.GAP8, CPXTarget.HOST, cpx_data)
        st.writePacket(p)
        time.sleep(0.5)
        st.disconnect()
    

if __name__ == "__main__":
    main()
