from PIL import Image

f1 = open("aled.txt", "r")
f_labels = open("test/labels.txt", "r")
f_q = open("accuracy.log", "r")

l = f_labels.readlines()
labels = [int(ligne) for ligne in l]

l = f1.readlines()
mod = [int(ligne) for ligne in l]

l = f_q.readlines()
quant = [int(ligne) for ligne in l]

print(len(mod))
cpt = 0
for i, d in enumerate(mod):
    if mod[i] == labels[i]:
        cpt += 1
print(f"accuracy with labels: {cpt/len(mod)}")

cpt = 0
for i, d in enumerate(mod):
    if mod[i] == quant[i]:
        cpt += 1

print(f"accuracy with quantize: {cpt/len(mod)}")