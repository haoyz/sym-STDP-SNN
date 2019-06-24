# coding:utf-8
import struct
import numpy as np
import matplotlib.pyplot as plt
import string
import time

filename = '/home/hyz/Downloads/dataset/fashion-mnist/t10k-images-idx3-ubyte'
binfile = open(filename, 'rb')
buf = binfile.read()

errorIDbuf = np.loadtxt('./output/error_ID')
type(errorIDbuf)
len(errorIDbuf)  # 400*784
errorIDbuf_tran = np.transpose(errorIDbuf)  # 先行列交换 784*400

offset = 0
magic, numImages, numRows, numColumns = struct.unpack_from(
    '>IIII', buf, offset)  # 读取前4个字节的内容
offset += struct.calcsize('>IIII')
for j in range(len(errorIDbuf_tran[0])):
    index = offset + struct.calcsize('>784B') * int(errorIDbuf_tran[0][j])
    im = struct.unpack_from('>784B', buf, index)  # 以大端方式读取一张图上28*28=784
    im = np.array(im)
    im = im.reshape(28, 28)
    fig = plt.figure()
    plotwindow = fig.add_subplot(111)
    plt.axis('off')
    print(errorIDbuf[j][1])
    a = './output/test_' + \
        str(int(errorIDbuf[j][1])) + 'to' + str(int(errorIDbuf[j][2])) + '_'
    b = [a, '.png']
    savefile = str(int(errorIDbuf_tran[0][j])).join(b)
    print(savefile)
    plt.imshow(im, cmap='gray')
    # plt.show()#否则不能保存为文件
    plt.savefig(savefile)  # 保存成文件
    plt.close()
binfile.close()
# './dataset/train-images-idx3-ubyte'
# './dataset/t10k-images-idx3-ubyte'
