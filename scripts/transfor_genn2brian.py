import numpy as np


g_save = np.loadtxt("../src/output_gPE")
type(g_save)
len(g_save)  # 400*784
g_save = np.transpose(g_save)  # 先行列交换 784*400
_r = np.array(range(len(g_save)))  # 784
_col = np.array(range(len(g_save[0])))  # 400
#import itertools as its
# r_col=its.product(_r,_col)
# type(r_col)
# np.hstack((g_save,r_col))
g_save = [(i, j, g_save[i, j]) for i in _r for j in _col]  # 前面的先变后面后变
g_save = np.array(g_save, dtype=float)  # list2array
np.save("myXeAe.npy", g_save)


theta = np.loadtxt("../src/output_theta")
theta = theta / 1000
# theta=theta.reshape(1,400)#读出来就是1*400
np.save("mytheta_A.npy", theta)
