import matplotlib as plt
from pylab import *
import math
mpl.rcParams['font.sans-serif'] = ['SimHei']  # 添加这条可以让图形显示中文
x_axis_data = []
y_axis_data = []

v_init = 10.0
h_init = 50.0
x_init = 0.0

h = h_init
time = 0
# v = v_init + a*t 
# F = m*a
# 水平方向： F_x = k * v_x
# 垂直方向： F_y = k * v_y
def v_x(t):
    return 10.0/(1.0+t)  

def v_y(t):
    return 9.8*t/(1.0+t)

y = h_init
x = x_init
while y>=0:
    #通过微分，将整个轨迹分解为每0.0001s的直线运动
    y = 50-v_y(time)*time
    x = v_x(time)*time
    x_axis_data.append(x)
    y_axis_data.append(y)
    # print('x:%f'%x)
    # print('y:%f'%y)

    time = time + 0.0001


plt.figure(figsize = (16, 8))
plt.plot(x_axis_data, y_axis_data, 'r-', color='#4169E1', alpha=0.8, linewidth=1, label='轨迹')
plt.legend(loc="upper right")
plt.xlabel('水平距离')
plt.ylabel('高度')

plt.show()