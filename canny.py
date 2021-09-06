import cv2
from matplotlib import pyplot as plt

img = cv2.imread('1.jpg', 1)
# cv2.imshow('img', img)

GBlur = cv2.GaussianBlur(img, (5, 5), 0)
# cv2.imshow('1', GBlur)

ret, thresh_img = cv2.threshold(GBlur, 80, 255, cv2.THRESH_BINARY)
# cv2.imshow("2", thresh_img)

canny = cv2.Canny(GBlur, 50, 150)
cv2.imshow('canny', canny)

plt.imshow(canny, cmap='gray', interpolation='bicubic')
plt.xticks([]), plt.yticks([])
plt.show()

# k = cv2.waitKey(0) & 0xFF
# if k == 27:
#     cv2.destroyAllWindows()
# elif k == ord('s'):
#     cv2.imwrite('11.jpg',canny)
#     cv2.destroyAllWindows()

# cv2.destroyAllWindows()
cv2.waitKey(0)