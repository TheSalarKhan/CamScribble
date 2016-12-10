#include "PerspectiveCorrection.h"
using namespace cv;


PerspectiveCorrection::PerspectiveCorrection() {
  _params.tl =
    _params.tr =
    _params.bl =
    _params.br = Point2f(0.0f,0.0f);

  _params.width = _params.height = 0;

}

PerspectiveCorrection::PerspectiveCorrection(PerspectiveCorrectionParams p) {
  _params = p;
}

void PerspectiveCorrection::setSurfaceCorners(Point2f tl, Point2f tr, Point2f bl, Point2f br) {
  _params.tl = tl;
  _params.tr = tr;
  _params.bl = bl;
  _params.br = br;


}

void PerspectiveCorrection::setOutputWidth(int width) {
  if(width <= 0)
    return;
  _params.width = width;

  calculateTransformationMatrix();
}

void PerspectiveCorrection::setOutputHeight(int height) {
  if(height <= 0)
    return;
  _params.height = height;

  calculateTransformationMatrix();
}


void PerspectiveCorrection::applyPerspectiveCorrection(const Mat& inputImage,Mat& outputImage) {


  if(inputImage.rows != _inputImageRows || inputImage.cols != _inputImageCols) {
	  _inputImageCols = inputImage.cols;
	  _inputImageRows = inputImage.rows;
	  calculateTransformationMatrix();

  }


  warpPerspective(inputImage, outputImage, _transformationMatrix,
    Size(_params.width,_params.height));

}


// This function needs to be called whenever, input image size changes,
// or when the output image height changes.
void PerspectiveCorrection::calculateTransformationMatrix() {
	// tl,tr,bl,br are normalized between 1-0.
	// need to de-normalize - multiply with input image dimensions - them before calculating transformation matrix.

	const Point2f src[] = {
			Point2f(_params.tl.x * _inputImageCols,_params.tl.y * _inputImageRows),
			Point2f(_params.tr.x * _inputImageCols,_params.tr.y * _inputImageRows),
			Point2f(_params.bl.x * _inputImageCols,_params.bl.y * _inputImageRows),
			Point2f(_params.br.x * _inputImageCols,_params.br.y * _inputImageRows)
	};
	const Point2f dst[] = {
	Point2f(0.0f,0.0f),
	Point2f(_params.width,0.0f),
	Point2f(0.0f,_params.height),
	Point2f(_params.width,_params.height)
	};
	_transformationMatrix = cv::getPerspectiveTransform(src,dst);
}
