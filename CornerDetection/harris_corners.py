import cv2
import numpy as np
from getUserCorners import getCorners
import sys

sys.path.append('/home/salar/CURRENT_PROJECTS/FYP2')

from CamScribbleCore.Filters.PerspectiveCorrection import PerspectiveCorrection











cam = cv2.VideoCapture(1)


corners = getCorners(cam,4,"Please select 4 corners, by double clicking on the image for each one.")

perspectiveCorrection = PerspectiveCorrection()
perspectiveCorrection.setSurfaceCorners(corners[0],corners[1],corners[2],corners[3])
perspectiveCorrection.setOutputHeight(400)
perspectiveCorrection.setOutputWidth(600)





def algo(img):
    gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

    gray = np.float32(gray)
    dst = cv2.cornerHarris(gray, 2, 3, 0.04)

    # result is dilated for marking the corners, not important
    dst = cv2.dilate(dst, None)

    # Threshold for an optimal value, it may vary depending on the image.
    img[dst == dst.max()] = [0, 0, 255]

    cv2.imshow('map',dst)


    # for corner in res:
    #     cv2.circle(img, (corner[2], corner[3]), 2, (255, 0, 255), -1)

    return img

while(1):
    _,img = cam.read()

    img = perspectiveCorrection.applyPerspectiveCorrection(img)

    #img = cv2.medianBlur(img,5)
    img = cv2.GaussianBlur(img,(5,5),6)
    img = cv2.Laplacian(img,cv2.CV_8U)

    img[img > 1] = 255



    cv2.imshow('dst',algo(img))
    if cv2.waitKey(20) & 0xff == 27:
        break
cv2.destroyAllWindows()