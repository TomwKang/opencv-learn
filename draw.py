import numpy1 as np
import cv2 as cv

img = np.zeros((512,512,3), np.uint8)
# img = cv.imread('1.jpg',1)

cv.line(img,(0,0), (511,511), (255,0,0), 5)
cv.rectangle(img, (384,0), (510,128), (255,255,255), 3)
pts = np.array([[10,5],[20,30],[70,20],[50,10]], np.int32)
pts = pts.reshape((-1,1,2))
cv.polylines(img, [pts], True, (0,255,255))

font = cv.FONT_HERSHEY_SIMPLEX
cv.putText(img, 'HHH', (10,500), font, 4, (255,255,0),2,cv.LINE_AA)

cv.imshow('line',img)

cv.waitKey(0)
cv.destroyAllWindows()