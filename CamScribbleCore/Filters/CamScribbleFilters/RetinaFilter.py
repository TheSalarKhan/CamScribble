import cv2
import numpy as np
from ..BackgroundAveraging import RunningAverage


class RetinaFilter:
	"""
		This is the filter which is responsible of taking in
		the image from the perspective transform, and producing
		the output with text as white and background as black.
		It is called the retina filter because it removes random
		noise from the image with the help of background averaging,
		which is somewhat similar to how the human eye works.
	"""
	def __init__(self,learningRate=0.4):
		self._accumulator = RunningAverage(learningRate)

		# parameters used for adaptive thresholding.
		self._subtractFromMean = 6
		self._averageThreshGridSize = 13

		# parameter used for background averaging
		self._backgroundAverageThreshold = 25

		# parameter used for the max intensity
		# of the produced output.
		self.__desiredMaxIntensity = 255

	def setAdaptiveKernelSize(self,val):
		if(val % 2 == 1 and val > 1):
			self._averageThreshGridSize = val

	def setNoiseSupression(self,val):
		self._subtractFromMean = val

	def setBackgroundAveragingThreshold(self,val):
		self._backgroundAverageThreshold = val

	def setDesiredIntensity(self,val):
		if(val >= 0 and val <= 255):
			self.__desiredMaxIntensity = val

	def enhanceOutput(self,mask, originalImage):

		# bitwise and the output from the filter and the
		# original gray scale image. But add 1 to the grayscale image
		# before the and so that the darkest parts of ink which might be '0' in the
		# actual image do not get removed in the next step.
		ultimateOutput = cv2.bitwise_and(mask, originalImage + 1)

		# invert the image, so that the lighter edges are darker
		# and all the ink is bright.
		ultimateOutput = 255 - ultimateOutput

		# after the previous step all the background that was
		# black is now maximum white, '255'. So lets turn it
		# black again. This is the step where originalImage +1
		# is saved from being removed from the image.
		ultimateOutput[ultimateOutput == 255] = 0

		# find the maximum intensity in the image
		# if the maximum intensity is less than
		# 255, add (255 - maxIntensity) to the
		# whole image.
		maxIntensity = np.amax(ultimateOutput)

		toAdd = (self.__desiredMaxIntensity - maxIntensity)
		ultimateOutput = ultimateOutput + toAdd

		# there might be some negative values in the array
		# after the previous step, so lets set all the -ve values
		# to 0.
		ultimateOutput[ultimateOutput < 0] = 0

		ultimateOutput = ultimateOutput.astype(np.uint8)

		ultimateOutput = cv2.bitwise_and(mask, ultimateOutput)

		return ultimateOutput

	def getFrame(self,img):

		# convert to grayscale if not already
		if(len(img.shape) == 3):
			img = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

		# apply gaussian blur.
		img = cv2.GaussianBlur(img,(3,3),0)

		# save the original for the enhancement phase
		original = img.copy()

		# apply adaptive thresholding
		img = cv2.adaptiveThreshold(img,255,cv2.ADAPTIVE_THRESH_GAUSSIAN_C,\
            cv2.THRESH_BINARY,self._averageThreshGridSize,self._subtractFromMean) # these last two

		cv2.imshow('adaptiveThreshold',img);

		# accumulate the frame
		average = self._accumulator.getAverage(img)


		cv2.imshow('averageFrame',average);

		# threshold based on average frame values.
		constant = np.zeros(average.shape,dtype=np.uint8)
		constant[average < self._backgroundAverageThreshold] = 255


		cv2.imshow('constant',constant);

		return self.enhanceOutput(constant,original)