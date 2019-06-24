import numpy as np
import matplotlib.pyplot as plt
import itertools

w = np.loadtxt('./weights/gPE')
print type(w)
w = map(list, itertools.izip(*w))  # 列表行列转换方法3 map实现 不载入内存
w = np.array(w)
print type(w)
print len(w)
w = np.bmat([[w[i * 20 + j].reshape((28, 28))
              for j in range(20)] for i in range(20)])
print len(w)
plt.imshow(w, interpolation="nearest", vmin=0, vmax=1, cmap='hot_r')
plt.show()
