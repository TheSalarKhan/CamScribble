import cv2
import numpy as np
import sys

sys.path.append('/home/salar/CURRENT_PROJECTS/FYP2')

from CamScribbleCore.Filters.BackgroundAveraging import RunningAverage



cam = cv2.VideoCapture(1)

ba = RunningAverage(0.8)



def skeletonize(img):
    img = img*256
    img = np.asarray(img,dtype="uint8")
    size = np.size(img)
    element = cv2.getStructuringElement(cv2.MORPH_CROSS, (3, 3))
    skel = np.zeros(img.shape, np.uint8)
    done = False

    while (not done):

        eroded = cv2.erode(img, element)
        temp = cv2.dilate(eroded, element)

        temp = cv2.subtract(img, temp)
        skel = cv2.bitwise_or(skel, temp)
        img = eroded.copy()

        zeros = size - cv2.countNonZero(img)
        if zeros == size:
            done = True

    return img


while(1):
    _,img = cam.read()



    gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

    gray = cv2.GaussianBlur(gray, (5, 5), 0)

    edges = cv2.Canny(gray, 25, 25, apertureSize=3)

    #edges = ba.getAverage(edges)

    # remove noise
    # gray = cv2.GaussianBlur(gray, (5, 5), 0)
    #
    # # convolute with proper kernels
    # laplacian = cv2.Laplacian(gray, cv2.CV_32F)
    #
    #
    # sobelx = cv2.Sobel(gray, cv2.CV_32F, 1, 0, ksize=5)  # x
    # sobely = cv2.Sobel(gray, cv2.CV_32F, 0, 1, ksize=5)  # y
    #
    # edges = laplacian + sobelx + sobely
    #
    # edges[edges < 150] = 0


    # edges = np.asarray(edges,dtype="uint8")


    cv2.imshow('edges',edges)
    # minLineLength = 1
    # maxLineGap = 2

    # lines = cv2.HoughLines(edges, 1, np.pi / 180, 90)

    # try:
    #     for lin in lines:
    #         for rho, theta in lin:
    #             a = np.cos(theta)
    #             b = np.sin(theta)
    #             x0 = a * rho
    #             y0 = b * rho
    #             x1 = int(x0 + 1000 * (-b))
    #             y1 = int(y0 + 1000 * (a))
    #             x2 = int(x0 - 1000 * (-b))
    #             y2 = int(y0 - 1000 * (a))
    #
    #             cv2.line(img, (x1, y1), (x2, y2), (0, 0, 255), 1)
    #
    # except:
    #     pass





    # cv2.imshow('output',img)

    if cv2.waitKey(20) & 0xff == 27:
        break
cv2.destroyAllWindows()
#
# img = cv2.imread('dave.jpg')
# gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)
# edges = cv2.Canny(gray,50,150,apertureSize = 3)
# minLineLength = 100
# maxLineGap = 10
# lines = cv2.HoughLinesP(edges,1,np.pi/180,100,minLineLength,maxLineGap)
# for x1,y1,x2,y2 in lines[0]:
#     cv2.line(img,(x1,y1),(x2,y2),(0,255,0),2)
#
# cv2.imwrite('houghlines5.jpg',img)