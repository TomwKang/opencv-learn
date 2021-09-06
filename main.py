import numpy as np
import cv2 as cv
dataList_c = []

WIDTH = 500
HIGH = 500

def read_morphology(frame):
    """对图片进行形态学处理

    对输入图片进行多种形态学处理后返回

    arg：frame:输入的图片

    return:dst_dilate:处理后的图片 frame:大小变化后的原图
    """
    # 获取滑动条所设参数
    open_opr = cv.getTrackbarPos('open', 'mor_adjust')
    close_opr = cv.getTrackbarPos('close', 'mor_adjust')
    erode_opr = cv.getTrackbarPos('erode', 'mor_adjust')
    dilate_opr = cv.getTrackbarPos('dilate', 'mor_adjust')

    # 将图像进行大小处理并更改为HSV，同时显示二值后的效果
    frame = cv.resize(frame, (WIDTH, HIGH), interpolation=cv.INTER_CUBIC)
    mask = hsv_change(frame)
    cv.imshow("mask", mask)

    # 通过形态学算子对图片处理
    dst_open = open_binary(mask, open_opr, open_opr)
    dst_close = close_binary(dst_open, close_opr, close_opr)
    dst_erode = erode_binary(dst_close, erode_opr, erode_opr)
    dst_dilate = dilate_binary(dst_erode, dilate_opr, dilate_opr)

    # 圈定图片中心，可以用于做图片翻转
    # cv.circle(frame_new, (int(WIDTH / 2), int(HIGH / 2)), 2, (255, 0, 255), -1)

    return dst_dilate, frame


def creatTrackbar():
    """Create TrackBar for debug the arg

    create two trackbar:color_adjust and mor_adjust to search good arg
    :arg:
        none
    :return:
        none
    """

    def nothing(x):
        pass

    cv.namedWindow("color_adjust", cv.WINDOW_AUTOSIZE)
    cv.createTrackbar("hmin", "color_adjust", 63, 255, nothing)
    cv.createTrackbar("hmax", "color_adjust", 255, 255, nothing)
    cv.createTrackbar("smin", "color_adjust", 0, 255, nothing)
    cv.createTrackbar("smax", "color_adjust", 255, 255, nothing)
    cv.createTrackbar("vmin", "color_adjust", 235, 255, nothing)
    cv.createTrackbar("vmax", "color_adjust", 255, 255, nothing)

    cv.namedWindow("mor_adjust", cv.WINDOW_AUTOSIZE)
    cv.createTrackbar("open", "mor_adjust", 1, 30, nothing)
    cv.createTrackbar("close", "mor_adjust", 13, 30, nothing)
    cv.createTrackbar("erode", "mor_adjust", 3, 30, nothing)
    cv.createTrackbar("dilate", "mor_adjust", 12, 30, nothing)


# HSV颜色识别提取
def hsv_change(frame):
    """ hsv channel separation.

    :param frame: change
    :return:
    """
    hmin = cv.getTrackbarPos('hmin', 'color_adjust')
    hmax = cv.getTrackbarPos('hmax', 'color_adjust')
    smin = cv.getTrackbarPos('smin', 'color_adjust')
    smax = cv.getTrackbarPos('smax', 'color_adjust')
    vmin = cv.getTrackbarPos('vmin', 'color_adjust')
    vmax = cv.getTrackbarPos('vmax', 'color_adjust')
    hsv = cv.cvtColor(frame, cv.COLOR_BGR2HSV)
    # lower_hsv = np.array([hmin, smin, vmin])
    # upper_hsv = np.array([hmax, smax, vmax])
    lower_hsv = np.array([5, 5, 235])
    upper_hsv = np.array([52, 255, 255])
    mask = cv.inRange(hsv, lowerb=lower_hsv, upperb=upper_hsv)
    return mask


# 开：先腐蚀后膨胀--能够排除小亮点
def open_binary(binary, x, y):
    kernel = cv.getStructuringElement(cv.MORPH_RECT, (x, y))
    dst = cv.morphologyEx(binary, cv.MORPH_OPEN, kernel)
    return dst


# 闭：先膨胀后腐蚀--能够排除小黑点
def close_binary(binary, x, y):
    kernel = cv.getStructuringElement(cv.MORPH_RECT, (x, y))
    dst = cv.morphologyEx(binary, cv.MORPH_CLOSE, kernel)
    return dst


# 腐蚀
def erode_binary(binary, x, y):
    kernel = cv.getStructuringElement(cv.MORPH_RECT, (x, y))
    dst = cv.erode(binary, kernel)
    return dst


# 膨胀
def dilate_binary(binary, x, y):
    kernel = cv.getStructuringElement(cv.MORPH_RECT, (x, y))
    dst = cv.dilate(binary, kernel)
    return dst

def find_contours(binary, frame):  # find contours and main screening section
    contours, heriachy = cv.findContours(binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)

    length = len(contours)
    data_list = []
    first_data = []
    second_data1 = []
    second_data2 = []

    c = 0
    d = 0
    if length > 0:
        # print("---founding---")
        for i, contour in enumerate(contours):
            data_dict = dict()
            # print("countour", contour)
            area = cv.contourArea(contour)
            rect = cv.minAreaRect(contour)
            rx, ry = rect[0]
            rw = rect[1][0]
            rh = rect[1][1]
            z = rect[2]

            coor = cv.boxPoints(rect)
            x1 = coor[0][0]
            y1 = coor[0][1]
            x2 = coor[1][0]
            y2 = coor[1][1]
            x3 = coor[2][0]
            y3 = coor[2][1]
            x4 = coor[3][0]
            y4 = coor[3][1]

            # if i >= 1:
            data_dict["area"] = area
            data_dict["rx"] = rx
            data_dict["ry"] = ry
            data_dict["rh"] = rh
            data_dict["rw"] = rw
            data_dict["z"] = z
            data_dict["x1"] = x1
            data_dict["y1"] = y1
            data_dict["x2"] = x2
            data_dict["y2"] = y2
            data_dict["x3"] = x3
            data_dict["y3"] = y3
            data_dict["x4"] = x4
            data_dict["y4"] = y4
            data_list.append(data_dict)

        for i in range(len(data_list)):

            data_rh = data_list[i].get("rh", 0)
            data_rw = data_list[i].get("rw", 0)
            data_area = data_list[i].get("area", 0)
            if (float(data_rh / data_rw) >= 0.2) \
                    and (float(data_rh / data_rw) <= 4) \
                    and data_area >= 20:
                first_data.append(data_list[i])
            else:
                pass
        for i in range(len(first_data)):

            c = i + 1
            while c < len(first_data):
                data_ryi = float(first_data[i].get("ry", 0))
                data_ryc = float(first_data[c].get("ry", 0))
                data_rhi = float(first_data[i].get("rh", 0))
                data_rhc = float(first_data[c].get("rh", 0))
                data_rxi = float(first_data[i].get("rx", 0))
                data_rxc = float(first_data[c].get("rx", 0))

                if (abs(data_ryi - data_ryc) <= 3 * ((data_rhi + data_rhc) / 2)) \
                        and (abs(data_rhi - data_rhc) <= 0.2 * max(data_rhi, data_rhc)) \
                        and (abs(data_rxi - data_rxc) <= (6 / 2) * ((data_rhi + data_rhc) / 2)):

                    second_data1.append(first_data[i])
                    second_data2.append(first_data[c])

                c = c + 1

        # for i in range(len(second_data1)):
        #     data_z1 = second_data1[i].get("z", 0)
        #     data_z2 = second_data2[i].get("z", 0)
        #     if abs(data_z1 - data_z2) <= 6:
        #         third_data1.append(second_data1[i])
        #         third_data2.append(second_data2[i])

        if len(second_data1):
            global dataList_c
            dataList_c.clear()
            for i in range(len(second_data1)):

                rectangle_x1 = int(second_data1[i]["x1"])
                rectangle_y1 = int(second_data1[i]["y1"])
                rectangle_x2 = int(second_data2[i]["x3"])
                rectangle_y2 = int(second_data2[i]["y3"])

                if abs(rectangle_y1 - rectangle_y2) <= (6 / 2) * (abs(rectangle_x1 - rectangle_x2)):

                    global point1_1x, point1_1y, point1_2x, point1_2y, point1_3x, point1_3y, point1_4x, point1_4y
                    global point2_1x, point2_1y, point2_2x, point2_2y, point2_3x, point2_3y, point2_4x, point2_4y

                    point1_1x = second_data1[i]["x1"]
                    point1_1y = second_data1[i]["y1"]
                    point1_2x = second_data1[i]["x2"]
                    point1_2y = second_data1[i]["y2"]
                    point1_3x = second_data1[i]["x3"]
                    point1_3y = second_data1[i]["y3"]
                    point1_4x = second_data1[i]["x4"]
                    point1_4y = second_data1[i]["y4"]

                    point2_1x = second_data2[i]["x1"]
                    point2_1y = second_data1[i]["y1"]
                    point2_2x = second_data1[i]["x2"]
                    point2_2y = second_data1[i]["y2"]
                    point2_3x = second_data1[i]["x3"]
                    point2_3y = second_data1[i]["y3"]
                    point2_4x = second_data1[i]["x4"]
                    point2_4y = second_data1[i]["y4"]

                    if point1_1x > point2_1x:
                        pass

                        cv.rectangle(frame, (point2_2x, point2_2y), (point1_4x, point1_4y), (255, 255, 0), 2)

                    else:
                        point1_1x, point2_1x = point2_1x, point1_1x
                        point1_2x, point2_2x = point2_2x, point1_2x
                        point1_3x, point2_3x = point2_3x, point1_3x
                        point1_4x, point2_4x = point2_4x, point1_4x

                        point1_1y, point2_1y = point2_1y, point1_1y
                        point1_2y, point2_2y = point2_2y, point1_2y
                        point1_3y, point2_3y = point2_3y, point1_3y
                        point1_4y, point2_4y = point2_4y, point1_4y
                        cv.rectangle(frame, (point2_1x, point2_1y), (point1_3x, point1_3y), (255, 255, 0), 2)

                    cv.putText(frame, "target1:", (rectangle_x2, rectangle_y2 - 5), cv.FONT_HERSHEY_SIMPLEX,
                               0.5, [255, 255, 255])
                    center = (int((point2_2x + point1_4x) / 2), int((point2_2y + point1_4y) / 2))
                    cv.circle(frame, center, 2, (0, 0, 255), -1)  # 画出重心

                    dataList_c.append(center)
                    high = (rectangle_y1, rectangle_y2)
                    # dataRange.append(high)
        else:
            print("---not find---")

            dataList_c.clear()
    data_list.clear()


if __name__ == '__main__':
    img = cv.imread('red1.png', cv.IMREAD_COLOR)
    # temp = Camera()
    # temp = cv.imread("useless.png")
    creatTrackbar()

    while cv.waitKey(1):
        # img = temp.getp()
        binary, frame = read_morphology(img)
        # find_contours(binary, frame)
        contours, heriachy = cv.findContours(binary, cv.RETR_EXTERNAL, cv.CHAIN_APPROX_SIMPLE)
        cv.drawContours(frame, contours, -1, (0, 255, 0), 3)
        cv.imshow("binary", binary)
        cv.imshow("frame", frame)

# # b, g, r = cv.split(img)
# # # clr = input("R or B ?")
# # clr ='B'
# # if clr == 'R':
# #     gray_img = r
# # else:
# #     gray_img = b
#
# GBlur = cv.GaussianBlur(img, (5, 5), 0)
# # cv.imshow('blur',GBlur)
#
# hsv = cv.cvtColor(GBlur, cv.COLOR_BGR2HSV)
# lower_hsv = np.array([0, 0, 46])
# upper_hsv = np.array([50, 255, 255])
# gray_img = cv.inRange(hsv, lowerb=lower_hsv, upperb=upper_hsv)
# cv.imshow('gray',gray_img)
#
# ret, thresh_img = cv.threshold(gray_img, 60, 255, cv.THRESH_BINARY)
# # cv.imshow('thre',thresh_img)
#
# kernel = np.ones((3,3),np.uint8)
# dil_img = cv.dilate(thresh_img, kernel, iterations=1)
# # cv.imshow('dil',dil_img)
#
# contours, hierarchy = cv.findContours(dil_img, cv.RETR_TREE, cv.CHAIN_APPROX_SIMPLE)
# cv.drawContours(img, contours, -1, (0,255,0), 3)
#
# # find_contours(dil_img,img)
# cv.imshow('img',img)
#
# cv.waitKey(100000)
# cv.destroyAllWindows()