import cv2
import numpy as np

class RunningAverage:
	def __init__(self,threshold):
		self._readFirstFrame = False;
		self._threshold = threshold;

	def getAverage(self,img):
		# for the first time
		if(self._readFirstFrame is False):
			self._readFirstFrame = True;
			self._avg = np.float32(img);
			return cv2.convertScaleAbs(self._avg);
		
		cv2.accumulateWeighted(img,self._avg,self._threshold);
		return cv2.convertScaleAbs(self._avg);