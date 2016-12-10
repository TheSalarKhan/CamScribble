#ifndef __RETINA_FILTER_H__
#define __RETINA_FILTER_H__

#include "OpenCV.h"
#include "PerspectiveCorrection.h"
#include "RunningAverage.h"

using namespace cv;
class RetinaFilter {
public:
  RetinaFilter();
  RetinaFilter(float learningRate);

  void setAdaptiveKernelSize(int val);
  void setNoiseSupressionLeveL(int val);
  void setBackgroundAveragingThreshold(int val);
  void setBrightness(int val);
  void applyFilter(const Mat& inputImage, Mat& alphaMask, Mat& outputImage);
  PerspectiveCorrection& getPerspectiveCorrection();

private:
  int _subtractFromMean;
  int _averageThreshGridSize;
  int _backgroundAverageThreshold;
  int _desiredMaxIntensity;
  PerspectiveCorrection _persp;
  RunningAverage _accumulator;

  void enhanceOutput(Mat& mask, Mat& originalImage, Mat& outputImage);
};

#endif
