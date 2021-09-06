import numpy1 as np
import cv2 as cv

cap = cv.VideoCapture('1.mp4')

fourcc = cv.VideoWriter_fourcc(*'XVID')
out = cv.VideoWriter('output.avi', fourcc, 20.0, (640, 480))

while cap.isOpened():
    ret, frame = cap.read()
    if not ret:
        print("No rcv")
        break
    frame = cv.flip(frame, 0)
    out.write(frame)

    # gray = cv.cvtColor(frame, cv.COLOR_BGR2GRAY)
    cv.imshow('frame', frame)
    if cv.waitKey(1) == ord('q'):
        break
out.release()
cap.release()
cv.destroyAllWindows()