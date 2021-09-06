import cv2 as cv
import numpy as np


cap = cv.VideoCapture('1.mp4')
color = 'B'
last_point = None

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("No rcv")
        break
    frame = cv.resize(frame,(1000,600))
    # cv.imshow('frame', frame)

    # ---------------图像处理-------------#
    # pictrue = frame
    # ---------------1.红蓝颜色-------------#
    img = frame.copy()
    blur_img = cv.GaussianBlur(img, (5, 5), 75)
    hsv_img = cv.cvtColor(blur_img, cv.COLOR_BGR2HSV)
    if color == 'R':
        lower_value = np.array([0, 100, 100])
        upper_value = np.array([10, 255, 255])
    else:
        lower_value = np.array([80, 100, 200])
        upper_value = np.array([124, 255, 255])
    mask_img = cv.inRange(hsv_img, lower_value, upper_value)
    if color == 'R':
        lower_value = np.array([156, 100, 100])
        upper_value = np.array([180, 255, 255])
        mask_red2_img = cv.inRange(hsv_img, lower_value, upper_value)
        mask_img = mask_red2_img + mask_img
    cv.imshow('frame-color', mask_img)
    # ---------------2.二值化-------------#
    ret, thresh_img = cv.threshold(mask_img, 100, 255, cv.THRESH_BINARY)
    # cv.imshow('frame-th', thresh_img)
    # ---------------3.形态学变换：开闭涨缩-------------#
    element = cv.getStructuringElement(cv.MORPH_RECT, (5, 5))
    dilate_img = cv.dilate(thresh_img, element)
    morph_img = cv.morphologyEx(dilate_img, cv.MORPH_CLOSE, element)
    morph_img = cv.erode(morph_img, element)
    cv.imshow('frame-morph', morph_img)
    # ----------图像处理结束----------#
    # img = morph_img  # 处理后的图像img


    contours, hierarchy = cv.findContours(morph_img, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
    # print(hierarchy)
    whole_cnt = {}  # 所有的轮廓{i:[rect]}
    for i, cnt in enumerate(contours):
        if cv.contourArea(cnt) < 100:
            continue
        if hierarchy[0][i][3] == -1:
            whole_cnt[i] = []
        else:
            try:
                whole_cnt[hierarchy[0][i][3]].append(i)
            except KeyError:
                pass
            # print(hierarchy[0][i][3])
            # print('0\n')

        # rect = cv.minAreaRect(cnt)
        # rect_points = cv.boxPoints(rect)
        # rect_points = np.int0(rect_points)
        # cv.drawContours(img,[rect_points], 0, (0, 255, 255), 3)
    #-----自己调的筛选参数-----#
    Fath_ratio = (1, 4) #父轮廓的矩形长宽比
    Area_ratio = (0.4, 0.6) #轮廓面积占矩形比例
    Armor_Area_ratio = (0.06, 0.25) #装甲板面积占矩形比例
    Armo_ratio = (0.8, 2.4) #装甲板长宽比

    R_ratio = (0.8, 1.5)
    R_center = []
    true_rect = []

    # last_rect[0]=0
    dire = 1
    # print("1\n1")
    # targets = []
    # print(whole_cnt)
    # print(0)


    for key, value in whole_cnt.items():
        rect = cv.minAreaRect(contours[key])
        area = cv.contourArea(contours[key])
        length = max(rect[1][0], rect[1][1])
        width = min(rect[1][0], rect[1][1])
        # print(len(value))
        # print("%.1f"%area)
        # print('\n')
        # print("%.1f" % (length / width))
        # rect_armor = cv.minAreaRect(contours[value[0]])
        # targets = cv.boxPoints(rect_armor)
        # targets = np.int0(targets)
        # cv.drawContours(img, [targets], 0, (0, 0, 255), 5)
        # 只有一个子轮廓的父轮廓才是目标装甲板
        if len(value) == 1 :
            # -----一个子轮廓可能为R标----#
            if R_ratio[0] < length / width < R_ratio[1] \
                and length < 50 \
                and width < 30 :

                R_rect = rect
                R_center = R_rect[0]
                # print(R_center)
                cv.circle(img, (int(R_center[0]), int(R_center[1])), 4, (0, 0, 255), 3)

            # print(1)
            # 4个约束条件：父轮廓外接矩形的长宽比 & 轮廓占外接矩形的比例 & 目标装甲板占外接矩形的比例 & 目标装甲板的长宽比
            if Fath_ratio[0] < length / width < Fath_ratio[1] \
                    and Area_ratio[0] < cv.contourArea(contours[key]) / (length * width) < Area_ratio[1] \
                    and Armor_Area_ratio[0] < cv.contourArea(contours[value[0]]) / (length * width) \
                    < Armor_Area_ratio[1]:
                # print('1: %.2f' % (float(length) / float(width)))
                # print('2: %.2f' % (cv.contourArea(contours[key]) / (length * width)))
                # print('3: %.2f' % (cv.contourArea(contours[value[0]]) / (length * width)))
                # print(cv.contourArea(contours[value[0]]))
                rect_armor = cv.minAreaRect(contours[value[0]])

                l_armor = max(rect_armor[1][0], rect_armor[1][1])
                w_armor = min(rect_armor[1][0], rect_armor[1][1])
                # print('4: %.2f' % (l_armor / w_armor))
                if Armo_ratio[0] < l_armor / w_armor < Armo_ratio[1]:
                    true_rect = rect_armor
                    angle = rect_armor[-1]
                    # print(0)
                    # print(angle)
                    # print(1)
                    # print(true_rect[0])
                    # if last_point and angle:
                    #     print(2)
                    #     print(last_point)
                    targets = cv.boxPoints(rect_armor)
                    targets = np.int0(targets)
                    print(targets)
                    # print(targets)
                    cv.drawContours(img, [targets], 0, (0, 255, 255), 3)
                    r_center = rect_armor[0]
                    last_point  = r_center
                    cv.circle(img, (int(r_center[0]), int(r_center[1])), 2, (0, 0, 255), 3)
                    # print(targets)
                    # print(2)
        elif len(value) == 0 :
            # -----一个子轮廓可能为R标----#
            if R_ratio[0] < length / width < R_ratio[1] \
                and length < 50 \
                and width < 30 :

                R_rect = rect
                R_center = R_rect[0]
                # print(R_center)
                cv.circle(img, (int(R_center[0]), int(R_center[1])), 4, (0, 0, 255), 3)


        # # 没有子轮廓的父轮廓可能是R标
        # elif len(value) == 0:
        #     # 限制R标的长宽比
        #     if R_Fath_ratio[0] < length / width < R_Fath_ratio[1]:
        #         R_edge.append((key, rect))

    cv.imshow('img', img)

    if cv.waitKey(1) == ord('q'):
        break

cap.release()
cv.destroyAllWindows()