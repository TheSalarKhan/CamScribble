import numpy as np
import cv2

pointIndex = 0

def getCorners(cam,numberOfPoints,message):
    # This is the array which will hold the
    # four points.
    pts = [(0,0),(0,0),(0,0),(0,0)]



    # local function for handling mouse click events
    def draw_circle(event, x, y, flags, param):
        global pointIndex
        if event == cv2.EVENT_LBUTTONDBLCLK:
            pts[pointIndex] = (x, y)
            pointIndex = pointIndex + 1


    # local function for letting the user select points
    def selectFourPoints():

        print message

        while (pointIndex != numberOfPoints):
            _, img = cam.read()

            for i in range(0,pointIndex):
                cv2.circle(img, (pts[i][0],pts[i][1]), 5, (0, 0, 255), 5)

            cv2.imshow('select four corners', img)
            key = cv2.waitKey(20) & 0xFF
            if key == 27:
                return False

        return True

    # create a named window.
    cv2.namedWindow('select four corners')

    # register mouse click callback.
    cv2.setMouseCallback('select four corners', draw_circle)

    if(selectFourPoints()):
        cv2.destroyAllWindows()
        return pts
    else:
        cv2.destroyAllWindows()
        return []


if __name__ == "__main__":
    cam = cv2.VideoCapture(1)


    pts = getCorners(cam,4,"Please select 4 corners")

    print pts

