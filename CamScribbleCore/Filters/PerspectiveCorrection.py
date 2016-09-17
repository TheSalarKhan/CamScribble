import numpy as np
import cv2
import sys


class PerspectiveCorrection:
	"""
		This is the object which removes perspective distortion
		from the input image.
	"""
	def __init__(self,pts=[(0,0),(0,0),(0,0),(0,0)],outputSize=[400,400]):
		"""
		:param pts: it is an array that holds 4 points, top-left, top-right
			bottom-left, and bottom-right respectively.
		:param outputSize: It is a list of two values which represent
			width, and the height of the output respectively.
		"""

		self.__pts = pts
		self.__outputSize = outputSize
		self.__M = None
		self.__calculateTransformationMatrix()


	def setOutputHeight(self, height):
		if (height <= 0):
			return
		self.__outputSize[1] = height
		self.__calculateTransformationMatrix()


	def setOutputWidth(self, width):
		if (width <= 0):
			return
		self.__outputSize[0] = width
		self.__calculateTransformationMatrix()


	def setSurfaceCorners(self, topLeft, topRight, bottomLeft, bottomRight):
		self.__pts = [topLeft, topRight, bottomLeft, bottomRight]
		self.__calculateTransformationMatrix()


	def __calculateTransformationMatrix(self):
		"""
			This private function calculates the transformation matrix that
				is used to remove perspective distortion in the function
				applyPerspectivecorrection().
		:return:
		"""
		pts1 = np.float32([
			[self.__pts[0][0], self.__pts[0][1]],
			[self.__pts[1][0], self.__pts[1][1]],
			[self.__pts[2][0], self.__pts[2][1]],
			[self.__pts[3][0], self.__pts[3][1]]])

		pts2 = np.float32([
			[0, 0],
			[self.__outputSize[0], 0],
			[0, self.__outputSize[1]],
			[self.__outputSize[0], self.__outputSize[1]]])

		self.__M = cv2.getPerspectiveTransform(pts1, pts2)


	def applyPerspectiveCorrection(self,image):
		"""

		:param image: the image from which to remove perspective distortion.
		:return: image without the perspective distortion.
		"""

		# apply perspective transformation.
		return cv2.warpPerspective(image, self.__M, (self.__outputSize[0], self.__outputSize[1]))