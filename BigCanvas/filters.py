import numpy as np
import cv2
import sys
from CamScribbleCore.Filters.CamScribbleFilters.RetinaFilter import RetinaFilter
from CamScribbleCore.Filters.PerspectiveCorrection import PerspectiveCorrection

class InkFilter:
	"""
		This class is at the core of this project.
		Because, it implements the algorithm which
		filters out everything from an image but
		the ink.
	"""
	def __init__(self):
		self._retinaFilter = RetinaFilter();


	def applyFilter(self,frame):
		"""
		This is the main function in this class. It receives a grayscale image
		preferably with the perspective distortion removed beforehand, and it
		returns an image which is a binary mask, where white areas represent ink
		and the black areas are mostly the surface.
		:param frame: the image on which to apply the filter
		:return: image after the filter has been applied.
		"""

		dst = frame;
		binaryMask = self._retinaFilter.getFrame(frame);

		

		# extract the text area from the equalized image
		dst = cv2.bitwise_and(dst,binaryMask)


		# invert the acquired image
		dst = 255 - dst


		# wherever there was 0 now there is 255 so
		# replace 255 with 0
		dst[dst==255] = 0

		
		return dst

	def setAdaptiveFilterSize(self, val):
		self._retinaFilter.setAdaptiveKernelSize(val);

	def setAdaptiveFilterThreshold(self, val):
		self._retinaFilter.setNoiseSupression(val);

	def setBackgroundAveragingRate(self, val):
		self._retinaFilter.setBackgroundAveragingThreshold(val);