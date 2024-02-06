
import onnxruntime as ort
import numpy as np
from nntool.api import NNGraph
import natsort
import cv2


ort_session = ort.InferenceSession("./models/RUCO.onnx")

for i in range(0, (len(labels))):
    img = cv2.imread(f"{dataset_path}/{i:05d}.ppm")
    d = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    d = d.astype(np.int32)

    data_onnx = np.reshape(d, (1, 1, 200, 200))
    data_onnx = data_onnx.astype(np.int32) - 128
    outputs = ort_session.run(
        None,
        {"input": list(data_onnx.astype(np.int8))},
    )
    out_onnx.append(outputs[0][0][0])
    print(f"{i}: {outputs[0][0][0]}")