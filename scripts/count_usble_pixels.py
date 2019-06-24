# coding:utf-8
import struct
import numpy as np
import matplotlib.pyplot as plt
import string
import time

filename = '/home/hyz/Downloads/dataset/fashion-mnist/t10k-images-idx3-ubyte_normalized_to30'
# filename = '/home/hyz/Downloads/dataset/fashion-mnist/train-images-idx3-ubyte'

binfile = open(filename, 'rb')
buf = binfile.read()

offset = 0
magic, numImages, numRows, numColumns = struct.unpack_from(
    '>IIII', buf, offset)  # 读取前4个字节的内容
offset += struct.calcsize('>IIII')
total_mean = 0
tatal_count = 0
for j in range(numImages):
    index = offset + struct.calcsize('>784B') * int(j)
    im = struct.unpack_from('>784B', buf, index)  # 以大端方式读取一张图上28*28=784
    im = np.array(im)
    total_mean += np.mean(im)
    tatal_count += sum(pixel > 0 for pixel in im)
binfile.close()
total_mean /= float(numImages)
tatal_count /= float(numImages)
print(total_mean)
print(tatal_count)
