import cv2
import numpy as np
from CamScribbleCore.Filters.CamScribbleFilters.RetinaFilter import RetinaFilter

cap = cv2.VideoCapture(1);	
# lonAvg = BackgroundAveraging(0.4);
myAverageFilter = RetinaFilter()

def meanChanged(val):
	myAverageFilter.setNoiseSupression(val);

def gridChanged(val):
	myAverageFilter.setAdaptiveKernelSize(val);

def runningAverageChanged(val):
	myAverageFilter.setBackgroundAveragingThreshold(val); 

cv2.namedWindow('Control Panel')
cv2.createTrackbar('Noise supression', 'Control Panel',0,100,meanChanged)
cv2.createTrackbar('Adaptive Kernel Size', 'Control Panel',0,100,gridChanged)
cv2.createTrackbar('Running average threshold', 'Control Panel',0,100,runningAverageChanged)



# lAvg1 = BackgroundAveraging(0.3);
# lAvg2 = BackgroundAveraging(0.5);
# lAvg3 = BackgroundAveraging(0.5);

counter = 0;

while(1):

	_,image = cap.read();

	# if(counter % 2 == 0):
		
	average = myAverageFilter.getFrame(image);

		# counter = 1;

		# continue;

	cv2.imshow('output',average);



		

	# counter = counter +1;
	# cv2.imshow('edges',res1);
	key = cv2.waitKey(10) & 0xFF;

	if(key == 27):
		break;


cap.release();

cv2.destroyAllWindows();