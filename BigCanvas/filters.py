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
		self._retinaFilter = RetinaFilter()


	def applyFilter(self,frame):
		"""
		This is the main function in this class. It receives a grayscale image
		preferably with the perspective distortion removed beforehand, and it
		returns an image which is a binary mask, where white areas represent ink
		and the black areas are mostly the surface.
		:param frame: the image on which to apply the filter
		:return: image after the filter has been applied.
		"""
		return self._retinaFilter.getFrame(frame)

	def setAdaptiveFilterSize(self, val):
		self._retinaFilter.setAdaptiveKernelSize(val)

	def setAdaptiveFilterThreshold(self, val):
		self._retinaFilter.setNoiseSupression(val)

	def setBackgroundAveragingRate(self, val):
		self._retinaFilter.setBackgroundAveragingThreshold(val)

	def setDesiredIntensity(self,val):
		self._retinaFilter.setDesiredIntensity(val)