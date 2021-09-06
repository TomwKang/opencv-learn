import matplotlib as plt
from pylab import *
mpl.rcParams['font.sans-serif'] = ['SimHei']  # 添加这条可以让图形显示中文

x_axis_data = [1, 2, 3, 4, 5]
y_axis_data = [1, 2, 3, 4, 5]

# plot中参数的含义分别是横轴值，纵轴值，线的形状，颜色，透明度,线的宽度和标签
plt.plot(x_axis_data, y_axis_data, 'ro-', color='#4169E1', alpha=0.8, linewidth=1, label='一些数字')

# 显示标签，如果不加这句，即使在plot中加了label='一些数字'的参数，最终还是不会显示标签
plt.legend(loc="upper right")
plt.xlabel('x轴数字')
plt.ylabel('y轴数字')

plt.show()
# plt.savefig('demo.jpg')  # 保存该图片