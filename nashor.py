#----------------------------------
#       大小符能量机关-识别及预测程序
#               Tom
#            2021/8/21
#----------------------------------
import cv2 as cv
import numpy as np
from config import *
import math

# 获取点所在象限
def Quadrant(center, last_point, true_point):
    if (last_point[0] > center[0] and true_point[0] > center[0]) \
            and (last_point[1] < center[1] and true_point[1] < center[1]):
        return 1
    elif (last_point[0] < center[0] and true_point[0] < center[0]) \
            and (last_point[1] < center[1] and true_point[1] < center[1]):
        return 2
    elif (last_point[0] < center[0] and true_point[0] < center[0]) \
            and (last_point[1] > center[1] and true_point[1] > center[1]):
        return 3
    elif (last_point[0] > center[0] and true_point[0] > center[0]) \
            and (last_point[1] > center[1] and true_point[1] > center[1]):
        return 4
    else:
        return -1
# 获取转动方向
def direction(Quadrant, last_point, true_point):
    if Quadrant == 1 and last_point[1]<=true_point[1] and last_point[0]<=true_point[0] \
            or Quadrant == 2 and last_point[1]>=true_point[1] and last_point[0]<=true_point[0] \
            or Quadrant == 3 and last_point[1]>=true_point[1] and last_point[0]>=true_point[0] \
            or Quadrant == 4 and last_point[1]<=true_point[1] and last_point[0]>=true_point[0]:
        return 1
    else:
        return 0
# 获取两点间距离
def getDist_P2P(Point0, PointA):
    distance=math.pow((Point0[0]-PointA[0]),2) + math.pow((Point0[1]-PointA[1]),2)
    distance=math.sqrt(distance)
    return distance


#大符类：用于能量机关的识别与预测
class nashor:
    def __init__(self, color, v, stat, debug):
        self.color = color          # 颜色
        self.v = v                  # 弹速
        self.true_point = (0, 0)    # 此次装甲板中心
        self.pre_point = (0, 0)     # 预测打击点
        self.last_point = (0, 0)    # 上次装甲板中心
        self.distant = 10           # 距离
        self.pictrue = None         # 识别的图像
        self.direction = 0          # 能量机关旋转方向 0-顺时针，1-逆时针
        self.ture_angle = 0         # 此次偏转角度--装甲板角度
        self.last_angle = 0         # 上次偏转角度
        self.ture_rect = []         # 此次装甲板矩形
        self.pre_rect = []          # 预测的装甲板矩形
        self.last_rect = []         # 上次装甲板矩形
        self.R_rect = []            # R标位置
        self.R_center = []          # 转动圆心位置
        self.success = 0            # 是否识别到装甲板
        self.spd = 10               # 旋转角速度
        self.time = 0               # 上下两帧时间间隔
        self.num = 0                # 帧数
        self.stat = stat            # B--大符，S--小符
        self.debug = debug          # 0--不调试，1--调试

    def find(self,picture):
        """
        函数功能：图像处理--寻找目标装甲板位置--预测击打点
        返回：处理后的图像
        """
        # ---------------一、图像处理------------- #
        self.pictrue = picture

        # ---------------1.红蓝颜色------------- #
        img = picture.copy()
        blur_img = cv.GaussianBlur(img, (5, 5), 75)  # 高斯滤波
        hsv_img = cv.cvtColor(blur_img,cv.COLOR_BGR2HSV)  # 将图像转为HSV，便于颜色提取
        if self.color == 'R':
            lower_value = np.array([0, 100, 100])
            upper_value = np.array([10, 255, 255])
        else:
            lower_value = np.array([80, 100, 200])
            upper_value = np.array([124, 255, 255])
        mask_img = cv.inRange(hsv_img, lower_value,upper_value)
        if self.color == 'R':  # 红色有两个区间，再次进行提取，并合并
            lower_value = np.array([156, 100, 100])
            upper_value = np.array([180, 255, 255])
            mask_red2_img = cv.inRange(hsv_img, lower_value,upper_value)
            mask_img = mask_red2_img + mask_img

        # ---------------2.二值化------------- #
        ret, thresh_img = cv.threshold(mask_img, 100, 255, cv.THRESH_BINARY)

        # ---------------3.形态学变换：开闭涨缩------------- #
        element = cv.getStructuringElement(cv.MORPH_RECT, (5, 5))
        dilate_img = cv.dilate(mask_img, element)
        morph_img = cv.morphologyEx(dilate_img, cv.MORPH_CLOSE, element)
        # ----------图像处理结束---------- #

        # -------------二、寻找目标扇叶---------- #
        contours, hierarchy = cv.findContours(morph_img, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE) # 第二个参数可以找出父子轮廓
        whole_cnt = {} # 所有的轮廓{i:[rect]}

        # 第一次筛选
        min_area = 50  # 最小轮廓面积，将小面积的非扇叶轮廓去掉
        for i, cnt in enumerate(contours):
            # -------根据轮廓面积筛选出过小的轮廓------ #
            if cv.contourArea(cnt)<min_area:
                continue
            # -------根据子轮廓筛选：此变量代表子轮廓个数，如果为-1则表示无子轮廓------ #
            if hierarchy[0][i][3] == -1:
                whole_cnt[i] = []
            else:
                # -------有子轮廓将其加进一个dict中------ #
                try:
                    whole_cnt[hierarchy[0][i][3]].append(i)
                except KeyError:  # 会出现这个错误
                    pass

        # 第二次筛选
        for key,value in whole_cnt.items():

            rect = cv.minAreaRect(contours[key])  # 扇叶外接矩形
            # 扇叶外接矩形长宽
            length = max(rect[1][0],rect[1][1])
            width = min(rect[1][0], rect[1][1])
            # 筛选条件1、2
            fath_ratio = length / width     # 父轮廓的矩形长宽比
            area_ratio = cv.contourArea(contours[key]) / (length*width)     # 轮廓面积占矩形比例
            # ----目标扇叶的子轮廓为1--- #
            if len(value) == 1 :
                # -----根据四个条件筛选扇叶与装甲板---- #
                armor_rect = cv.minAreaRect(contours[value[0]])  # 装甲板矩形
                # 装甲板矩形长宽
                armor_len = max(armor_rect[1][0], armor_rect[1][1])
                armor_wid = min(armor_rect[1][0], armor_rect[1][1])
                # 筛选条件3、4
                armor_area_ratio = cv.contourArea(contours[value[0]]) / (length * width)  # 装甲板面积占矩形比例
                armor_ratio = armor_len / armor_wid  # 装甲板长宽比
                self.success = 0
                if Fath_ratio[0] < fath_ratio < Fath_ratio[1] \
                        and Area_ratio[0] < area_ratio < Area_ratio[1] \
                        and Armor_Area_ratio[0] < armor_area_ratio < Armor_Area_ratio[1] \
                        and Armor_ratio[0] < armor_ratio < Armor_ratio[1] :

                    self.ture_angle = armor_rect[-1]
                    self.ture_rect = cv.boxPoints(armor_rect)
                    self.ture_rect = np.int0(self.ture_rect) # 目标装甲板位置
                    self.true_point = armor_rect[0]     # 目标装甲板中心点

                    self.success = 1
                # -----一个子轮廓可能为R标---- #
                if R_ratio[0] < length / width < R_ratio[1] \
                        and length < 50 \
                        and width < 30 :
                    self.R_rect = rect

            elif len(value) == 0:
                # -----无子轮廓可能为R标---- #
                if R_ratio[0] < length / width < R_ratio[1] \
                        and length < 50 \
                        and width < 30:
                    self.R_rect = rect

        # -------------三、预测击打点位置---------- #
        if self.last_angle and self.success:
            if self.stat == 'B':  # 如果是大符模式，需要测量风车旋转速度
                self.spdChange()
                # 测速  默认小符10rpm，飞行时间1s，转角为60°
            self.detect()  # 预测

        # -------------四、记录此次数据并返回处理后的图像---------- #
        self.last_point = self.true_point
        self.last_rect = self.ture_rect
        self.last_angle = self.ture_angle
        return img

    def detect(self):
        """
        函数功能：预测装甲板、击打点位置
        """

        # 判断转动方向
        point = [self.last_point, self.true_point]
        Q = Quadrant(self.R_center,point[0],point[1])
        if Q != -1:
            self.direction = direction(Q, point[0], point[1])

        # 预测击打装甲板位置、中心点位置
        theta = self.theta()  # 转动角
        rot_mat = cv.getRotationMatrix2D(self.R_center, float(theta), 1)
        sinA = rot_mat[0][1]
        cosA = rot_mat[0][0]
        self.pre_point = self.get_rot_point(sinA,cosA,self.true_point)  # 将装甲板中心点旋转指定角度，得到预测打击点

    def draw(self,pictrue):
        """
        函数功能：画出装甲板、击打点、轨迹圆
        返回：画后的图像
        """
        ture_rect = self.ture_rect  # 真实装甲板位置
        r_center = self.true_point  # 装甲板中心点
        self.R_center = self.R_rect[0]  # R标-旋转中心位置
        p_center = self.pre_point  # 预测击打点
        if self.debug:
            # 画出真实装甲板位置
            cv.drawContours(pictrue, [ture_rect], 0, (0, 255, 255), 3)
            # 画出装甲板中心点
            if r_center:
                cv.circle(pictrue, (int(r_center[0]), int(r_center[1])), 2, (0, 0, 255), 3)
            # 画出击打点圆形轨迹
            if self.R_center:
                r = int(getDist_P2P(self.pre_point, self.R_center))
                cv.circle(pictrue, (int(self.R_center[0]), int(self.R_center[1])), r, (255, 255, 255), 1)
            # 画出R标-旋转中心位置
            if self.R_rect:
                cv.circle(pictrue, (int(self.R_center[0]), int(self.R_center[1])), 2, (0, 0, 255), 2)
        # 画出预测击打点
        cv.circle(pictrue, (int(p_center[0]), int(p_center[1])), 4, (0, 0, 255), 4)
        return pictrue

    def theta(self):
        """
        函数功能：测量转动角度差
        返回：转动角度差
        """
        return ((float(self.distant) / self.v) * self.spd * np.pi / 30) * (180 / np.pi)

    def get_rot_point(self,sinA, cosA, p):
        """
        函数功能：将p点按角度A旋转
        返回：旋转后的点
        """
        center = self.R_center
        xx = -(center[0] - p[0])
        yy = -(center[1] - p[1])
        return (
            int(center[0] + cosA * xx - sinA * yy),
            int(center[1] + sinA * xx + cosA * yy))

    def spdChange(self):
        """
        函数功能：实时检测能量机关风车转速
        """
        if self.ture_angle - self.last_angle:
            if (self.num+1)/2 >= 0:  # 由于处理图像造成的延迟，所以通过此种方法达到隔20帧（约0.3s）计算一次转速，使预测图像较为稳定
                spd = (self.ture_angle - self.last_angle)/(self.time*6)  # 10转/分--60°/s，换算可得x转/分--6x°/s，time为上下两帧时间间隔
                if spd > -50 and spd < 50 and abs(spd-self.spd)<20:
                    self.spd = spd


