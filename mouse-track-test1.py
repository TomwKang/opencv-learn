import cv2 as cv
import numpy1 as np

def no(x):
    pass

global r,g,b
    
def draw_back(event,x,y,flags,param):
    global r, g, b
    if event == cv.EVENT_LBUTTONDBLCLK:
        cv.circle(img, (x,y),100,(r,g,b))

img = np.zeros((512,512,3),np.uint8)
cv.namedWindow('image')
cv.setMouseCallback('image',draw_back)

cv.createTrackbar('r', 'image', 0, 255, no)
cv.createTrackbar('g', 'image', 0, 255, no)
cv.createTrackbar('b', 'image', 0, 255, no)

while(1):
    cv.imshow('image',img)
    r = cv.getTrackbarPos('r','image')
    g = cv.getTrackbarPos('g', 'image')
    b = cv.getTrackbarPos('b', 'image')
    if cv.waitKey(20) & 0xFF == 27:
        break
cv.destroyAllWindows()