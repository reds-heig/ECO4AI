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

import argparse

import os

import torch
import torch.nn as nn
from BOURRICOT import BOURRICOT


WIFI_CTRL_STATUS_CLIENT_CONNECTED = 0x32
WIFI_CTRL_DATA_TRANSFER           = 0x33
MODEL_PATH = 'v1_11_ft.pth'
DEQUANTIZE_CONST = [
    0.0078125,
    0.01719304144851805,  # S4
    0.022580276324054387, # S7
    0.028325721034853476, # S10
    0.03353958430252676,  # S13
    0.03202371146735244,  # S16
    0.01226509225134756,  # S19
    0.0053388562856935984 # S22
]

CP_MAP = [ 0, 1, 3, 5, 7, 9, 11, 13, 15]


parser = argparse.ArgumentParser(description='Connect to AI-deck JPEG streamer example')
parser.add_argument("-n",  default="192.168.4.1", metavar="ip", help="AI-deck IP")
args = parser.parse_args()

deck_port = 5000
deck_ip = args.n
cpt = 0


def receive_data(st):
    # global cpt
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

    # tensor_l = list(tensor_data)
    tensor_l = np.frombuffer(tensor_data, dtype=np.int8)
    tensor_l = np.reshape(tensor_l, (1, channel, height, width))

    print("tensor_data shape: ", tensor_l.shape)

    return width, height, channel, cp, tensor_l

def dequantize(data, layer):
    # todo: dequantize
    data = data.astype(float)
    data = data*DEQUANTIZE_CONST[layer]
    data = torch.from_numpy(data)
    data = data.to(torch.float32)
    return data

def end_inference(model, data, cp):
    
    if cp > 6:
        model_output = model.classifier[cp-6+1:](data)
    else:    
        # todo: do end inference
        features_output = model.features[CP_MAP[cp]:](data)
        features_output = features_output.view(-1, 10000)
        model_output = model.classifier(features_output)
        # print(model_output[0][0].item())
    
    model_output = torch.Tensor.int(torch.relu(abs(model_output) / model_output)).type(torch.uint8) # binary step activation function
    return model_output[0][0].item()

def get_model(model, path):
    model.load_state_dict(torch.load(path)['model_state_dict'])
    return model

def main():    
    model = BOURRICOT(binarize_output=True)
    model = get_model(model, MODEL_PATH)
    # print(end_inference(model, torch.rand(1,1,200,200),0))
    st = SocketTransport(deck_ip, 5000)
    cpt = 0

    inf_time =[
        0,0,0,0,0,0,0,0,0
    ]
    nb_cp =[
        0,0,0,0,0,0,0,0,0
    ]
    try:
        while True:

            width, height, channel, cp, data = receive_data(st)

            start = time.time()
            data = dequantize(data, cp)
            out = end_inference(model, data, cp)
            end = time.time()

            inf_time[cp] += (end - start)
            nb_cp[cp] += 1

            cpx_data = struct.pack('<Bb', WIFI_CTRL_DATA_TRANSFER, out)
            p = CPXPacket(CPXFunction.WIFI_CTRL, CPXTarget.GAP8, CPXTarget.HOST, cpx_data)

            if cpt > 100:
                print(f"Waiting {200/1000}s")
                time.sleep(200/1000)
            # elif cpt > 200:
            #     time.sleep(200/1000)
            # elif cpt > 300:
            #     time.sleep(300/1000)
            # elif cpt > 200:
            #     time.sleep(200/1000)
            # elif cpt > 100:
            #     time.sleep(100/1000)

            cpt += 1
            st.writePacket(p)
            cv2.waitKey(1)
    finally:
        print("EXIT")

        for i, t in enumerate(inf_time):
            print(f"{nb_cp[i]}x TIME CP[{i}] = {t/nb_cp[i]}")

        cpx_data = struct.pack('<Bb', WIFI_CTRL_STATUS_CLIENT_CONNECTED, 0)
        p = CPXPacket(CPXFunction.WIFI_CTRL, CPXTarget.GAP8, CPXTarget.HOST, cpx_data)
        st.writePacket(p)
        time.sleep(0.5)
        st.disconnect()
    

if __name__ == "__main__":
    main()
