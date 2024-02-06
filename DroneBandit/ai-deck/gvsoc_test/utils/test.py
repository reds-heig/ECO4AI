#!/usr/bin/env python
# -*- coding: utf-8 -*- 
import cv2
import numpy as np

import onnx
import onnxruntime as ort
import os

def local_inference(input):

    # model = onnx.load("./models/WINSTON.onnx")
    ort_session = ort.InferenceSession("./models/WINSTON.onnx")
    data = np.reshape(input, (1, 1, 200, 200))
    # print(data)
    data2 = data.astype(np.float) - 128
    data3 = data.astype(np.int32) - 128
    # print("1--------------------------------------------")
    # print(data2)
    # print("2--------------------------------------------")
    # print(data3)
    # print("3--------------------------------------------")
    # print(data3.astype(np.float))
    # print("4--------------------------------------------")
    # print(data)

    outputs = ort_session.run(
        None,
        {"input": list(data3.astype(np.int8))},
    )
    return outputs

def main():
    path = "test/ppm"
    for i in range(0, 500):
        img = cv2.imread(f"{path}/image_{i}.ppm")
        img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
        ret = local_inference(img_gray)
        print(f"{ret[0][0][0]}")
        # print(f"{path}/frame_{i:05}.ppm: {ret[0][0][0]}")


    

if __name__ == "__main__":
    main()
