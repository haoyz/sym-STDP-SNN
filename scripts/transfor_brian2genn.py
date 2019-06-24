import numpy as np


theta_A = np.load("theta_A.npy")
theta_A = theta_A * 1000
np.savetxt("output_theta.txt", theta_A)


XeAe = np.load("XeAe.npy")
XeAe = XeAe[:, 2]  # 所有行的第三个单元元素
XeAe = XeAe.reshape(784, 400)  # 784行400列
len(XeAe)
# XeAe=[[r[col] for r in XeAe] for col in range(len(XeAe[0]))]#列表行列转换方法1 使用列表推导
# XeAe=map(list,zip(*XeAe))#列表行列转换方法2 map实现 载入内存
# import itertools
# XeAe = map(list, itertools.izip(*XeAe))  # 列表行列转换方法3 map实现 不载入内存
# len(XeAe)
np.savetxt("output_gPE.txt", XeAe)

XeAe = np.load("resultPopVecs10000.npy")#[10000][400]
np.savetxt("output_gPE_init.txt", XeAe)