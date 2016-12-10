#include "RetinaFilter.h"
using namespace cv;


RetinaFilter::RetinaFilter() {
  _accumulator = RunningAverage(0.4f);
  _subtractFromMean = 6;
  _averageThreshGridSize = 13;
  _backgroundAverageThreshold = 25;
  _desiredMaxIntensity = 255;
}

RetinaFilter::RetinaFilter(float learningRate) {
  _accumulator = RunningAverage(learningRate);
  _subtractFromMean = 6;
  _averageThreshGridSize = 13;
  _backgroundAverageThreshold = 25;
  _desiredMaxIntensity = 255;
}


void RetinaFilter::setAdaptiveKernelSize(int val) {
  if(val%2 ==1 && val >1 ) {
    _averageThreshGridSize = val;
  }
}

void RetinaFilter::setNoiseSupressionLeveL(int val) {
  _subtractFromMean = val;
}

void RetinaFilter::setBackgroundAveragingThreshold(int val) {
  _backgroundAverageThreshold = val;
}

void RetinaFilter::setBrightness(int val) {
  if(val >= 0 && val <= 255) {
    _desiredMaxIntensity = val;
  }
}

void RetinaFilter::enhanceOutput(Mat& mask, Mat& originalImage, Mat& outputImage) {

  // original Image should be grayscale.
  // bitwise the (original image + 1) with
  // mask, and invert to get intensity image.
  bitwise_and(mask, originalImage + 1, outputImage);
  outputImage = 255 - outputImage;

  // Where ever there is 255 set it to 0
  threshold(outputImage, outputImage, 244, 0, CV_THRESH_TOZERO_INV);

  // find maximum value in image.
  double mx,temp;
  minMaxLoc(outputImage, &temp, &mx);
  int max = mx;


  int intensityToAdd = _desiredMaxIntensity - max;

  outputImage = outputImage + intensityToAdd;

  // Any values less than zero should be set to zero.
  threshold(outputImage, outputImage, 0, 0, CV_THRESH_TOZERO);

  bitwise_and(mask, outputImage, outputImage);

}


void RetinaFilter::applyFilter(const Mat& inputImage,Mat& alphaMask,Mat& outputImage) {
	// the input image is a raw feed, its an RGB image.
	// alphaMask and outputImage are of the same dimension.

	// 1) Apply perspective transformation.
	_persp.applyPerspectiveCorrection(inputImage,alphaMask);


	// 2) convert to grayscale
	cvtColor(alphaMask,alphaMask,CV_BGR2GRAY);

	// 3) apply smoothing filter
	GaussianBlur(alphaMask,alphaMask,Size(3,3),0);

	// temporarily copy the input image to the output
	// image.
	alphaMask.copyTo(outputImage);

	// 4) Apply adaptive thresholding.
	// We have selected THRESH_BINARY,
	// so that the white bg, becomes 255,
	// and the black ink becomes 0.
	adaptiveThreshold(alphaMask,alphaMask,255,ADAPTIVE_THRESH_GAUSSIAN_C,THRESH_BINARY,_averageThreshGridSize,_subtractFromMean);

	// 5) Remove camera noise by averaging, with the last frames.
	// Pass it through averaging filter, to
	// reduce the static noise.
	_accumulator.getAverage(alphaMask,alphaMask);

	// 6) Threshold the average.
	// img[average < _backgroundAverageThreshold] = 255
	threshold(alphaMask,alphaMask,_backgroundAverageThreshold,255,THRESH_BINARY_INV);

	// 7) enhance output.
	enhanceOutput(alphaMask,outputImage,outputImage);

	// When control leaves this function, inputImage will have
	// the binary mask. and the outputImage will have the enhanced
	// version of the mask - which is the output.
}


PerspectiveCorrection& RetinaFilter::getPerspectiveCorrection() {
	return _persp;
}
