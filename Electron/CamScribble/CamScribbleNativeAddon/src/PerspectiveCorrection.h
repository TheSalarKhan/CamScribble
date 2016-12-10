#ifndef __PERSPECTIVE_CORRECTION_H__
#define __PERSPECTIVE_CORRECTION_H__

#include "OpenCV.h"
using namespace cv;

class PerspectiveCorrectionParams {
public:
  Point2f tl,tr,bl,br;
  int width,height;
};

class PerspectiveCorrection {
public:
  PerspectiveCorrection();
  PerspectiveCorrection(PerspectiveCorrectionParams p);

  void setSurfaceCorners(Point2f tl, Point2f tr, Point2f bl, Point2f br);
  void setOutputWidth(int width);
  void setOutputHeight(int height);

  void applyPerspectiveCorrection(const Mat& inputImage, Mat& outputImage);

private:
  PerspectiveCorrectionParams _params;
  Mat _transformationMatrix;
  int _inputImageCols,_inputImageRows;


  void calculateTransformationMatrix();
};

#endif
