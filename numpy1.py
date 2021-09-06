import numpy1 as np
import cv2 as cv

img = cv.imread('../fpx.jpg')


px = img[100,100]
print(px)

blue = img[100,100,0]
print(blue)

a = img[280:340,330:390]
img[273:333,100:160] = a
cv.imshow('img',img)

cv.waitKey(1000)
cv.destroyAllWindows()