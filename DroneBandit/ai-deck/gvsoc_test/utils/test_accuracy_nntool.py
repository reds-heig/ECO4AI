import os
import sys
import cv2
import onnxruntime as ort
import numpy as np
from nntool.api import NNGraph
import natsort

model_path = sys.argv[1]
dataset_path = sys.argv[2]
print(f"testing model {model_path} on dataset {dataset_path}")


G = NNGraph.load_graph(model_path)

file_list = natsort.natsorted(os.listdir(dataset_path))
data = []

out_nntool = []
out_onnx = []



# with file
f = open("./test/labels.txt", "r")
l = f.readlines()
labels = [int(ligne) for ligne in l]

print(len(labels))

# Get input dataset
# for f in file_list:
#     print(f)
#     img = cv2.imread(f"{dataset_path}/{f}")
#     img_gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#     data.append(img_gray)

# Execute default model if tes with tes acc should be 1.0
# G2 = NNGraph.load_graph("./models/WINSTON.onnx")
# for i, d in enumerate(data):
#     d = d.astype(np.int32) - 128
#     res = G2.execute(d, quantize=False)[-1][0][0]
#     if res == 127 or res == 1:
#         res = 1
#     else:
#         res = 0

#     out_nntool.append(res)
#     print(f"{i}: {res}")






# Execute quantized model
# for i in range(0, (len(labels))):
#     img = cv2.imread(f"{dataset_path}/{i:05d}.ppm")
#     d = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
#     d = d.astype(np.int32)
#     # print(d.astype(np.float))
#     res = G.execute(d.astype(np.float), quantize=True)[-1][0][0]
#     if res != 0:
#         res = 1
#     else:
#         res = 0

#     out_nntool.append(res)
#     print(f"{dataset_path}/{i:05d}.ppm: {res}")





f = open("bourriquot_out.txt", "r")
l = f.readlines()
out_nntool = [int(ligne) for ligne in l]





f = open("accuracy.log", "w")
f.write("nntool,onnx\n")
for i in range(0, len(out_nntool)):
    f.write(f"{out_nntool[i]}\n")

# Check accuracy
# ort_session = ort.InferenceSession("./models/RUCO.onnx")

# for i in range(0, (len(labels))):
#     data_onnx = np.reshape(d, (1, 1, 200, 200))
#     data_onnx = data_onnx.astype(np.int32) - 128
#     outputs = ort_session.run(
#         None,
#         {"input": list(data_onnx.astype(np.int8))},
#     )
#     out_onnx.append(outputs[0][0][0])
#     print(f"{i}: {outputs[0][0][0]}")


# Compare
# cpt = 0
cpt = [[0, 0],
       [0, 0]]
for i in range(0, len(out_nntool)):
    cpt[out_nntool[i]][labels[i]] += 1

acc = (cpt[0][0]+cpt[1][1])/len(out_nntool)

print("\t0\t1")
print(f"0\t{cpt[0][0]}\t{cpt[0][1]}")
print(f"1\t{cpt[1][0]}\t{cpt[1][1]}")
print(f"Accuracy = {acc}")
