#include "RunningAverage.h"
using namespace cv;


RunningAverage::RunningAverage() {}


RunningAverage::RunningAverage(float threshold) {
  _threshold = threshold;
}


void RunningAverage::getAverage(Mat& inputImage,Mat& outputImage){

  // If the size of the input image and
  // _avg is not same. Then discard the
  // already present _avg and replace it
  // with image. Else accumulate the frame
  if(inputImage.rows != _avg.rows || inputImage.cols != _avg.cols) {
	inputImage.convertTo(_avg,CV_32FC3, 1/255.0);
  } else {
	accumulateWeighted(inputImage, _avg, _threshold);
  }

  convertScaleAbs(_avg, outputImage);
}
