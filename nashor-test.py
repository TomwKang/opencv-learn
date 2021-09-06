#----------------------------------
#       大小符能量机关-测试程序
#               Tom
#            2021/8/21
#----------------------------------
import cv2 as cv
import numpy as np
import matplotlib.pyplot as plt
from nashor import *
import math
import time

cap = cv.VideoCapture('1.mp4')      # 读取视频
n = nashor("B", 20, 'B', 1)     # 创建能量机关类，并设定识别颜色为蓝色“B”, 初速度为20（B-大符，S-小符）, 大符模式, Debug模式（0-不开启，1-开启）

def getDist_P2P(Point0,PointA):
    distance=math.pow((Point0[0]-PointA[0]),2) + math.pow((Point0[1]-PointA[1]),2)
    distance=math.sqrt(distance)
    return distance

n.num = 0
while cap.isOpened():       # 如果视频打开成功
    start_time = time.time()    # 通过time函数计时
    ret, frame = cap.read()     # 读取视频图像
    if not ret:     # 读取失败则报错
        print("No rcv")
        break
    frame = cv.resize(frame,(1300,700))     # 为了显示方便更改图像尺寸
    # ------识别程序-------- #
    img = frame.copy()
    find_img = n.find(img)      # 查找目标装甲板
    if n.success:
        img = n.draw(find_img)      # 画出目标装甲板及其中心，预测击打点，轨迹圆
    cv.imshow("img", img)

    if cv.waitKey(1) == ord('q'):      # 通过waitKey控制帧率，按q退出程序
        break
    end_time = time.time()

    if n.num >= 10:  # 20帧改变一次
        n.time = end_time-start_time
        n.num = 0
    n.num += 1
    print('装甲板坐标：')
    print(n.ture_rect)
    print("time: %.0f ms\n" % ((end_time-start_time)*1000))

cap.release()       # 结束视频读取，释放
cv.destroyAllWindows()      # 关闭所有cv显示框

