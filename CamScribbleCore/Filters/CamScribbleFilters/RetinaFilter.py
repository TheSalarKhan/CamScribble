import cv2
import numpy as np
from ..BackgroundAveraging import RunningAverage

class RetinaFilter:
	"""
		This is the filter which is responsible of taking in
		the image from the perspective transform, and producing
		the output with text as white and background as black.
		It is called the retina filter because it removes random
		noise from the image with the help of background averaging.
	"""
	def __init__(self,learningRate=0.4):
		self._accumulator = RunningAverage(learningRate);

		# parameters used for adaptive thresholding.
		self._subtractFromMean = 6;
		self._averageThreshGridSize = 13;

		# parameters used for background averaging
		self._backgroundAverageThreshold = 25;

	def setAdaptiveKernelSize(self,val):
		if(val % 2 == 1 and val > 1):
			self._averageThreshGridSize = val;

	def setNoiseSupression(self,val):
		self._subtractFromMean = val;

	def setBackgroundAveragingThreshold(self,val):
		self._backgroundAverageThreshold = val;

	def getFrame(self,img):

		# convert to grayscale if not already
		if(len(img.shape) == 3):
			img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

		# apply gaussian blur.
		img = cv2.GaussianBlur(img,(3,3),0)

		# apply adaptive thresholding
		img = cv2.adaptiveThreshold(img,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,self._averageThreshGridSize,self._subtractFromMean) # these last two

		# accumulate the frame
		average = self._accumulator.getAverage(img);

		# threshold based on average frame values.
		constant = np.zeros(average.shape,dtype=np.uint8);
		constant[average < self._backgroundAverageThreshold] = 255;

		return constant;