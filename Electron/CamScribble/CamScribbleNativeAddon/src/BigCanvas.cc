/*
 * BigCanvas.cc
 *
 *  Created on: Nov 22, 2016
 *      Author: salar
 */

#include "BigCanvas.h"

BigCanvas::BigCanvas() {
	// TODO Auto-generated constructor stub

}


/**
 * This is a private helper function that is called whenever _smallCanvasHeight or
 * _smallCanvasWidth are changed.
 */
void BigCanvas::updateCanvasSize() {
	_retinaFilter.getPerspectiveCorrection().setOutputHeight((int)(_bigCanvas.rows-4)*_smallCanvasHeight);
	_retinaFilter.getPerspectiveCorrection().setOutputWidth((int)(_bigCanvas.cols-4)*_smallCanvasWidth);
}

BigCanvas::BigCanvas(Size sizeOfBigCanvas,Scalar backgroundColor,std::vector<Point2f> points) {

	// set foreground color
	_foregroundColor = Point3f(1.0f,1.0f,1.0f);


	// create retina filter obj.
	_retinaFilter = RetinaFilter();

	// initialize the position of the small canvas,
	// relative to the big canvas.
	_position = Point(0.0f,0.0f);

	// create the mat obj for the canvas, having the requested color
	// and dimensions.
	_bigCanvas = Mat(sizeOfBigCanvas,CV_8UC3,backgroundColor);

	// setup perspective transform
	setPerspective(points);


	// set settings for small canvas.
	_position = Point2f(0.5f,0.5f);
	_smallCanvasWidth = 0.5f;
	_smallCanvasHeight = 0.5f;
	updateCanvasSize();






}

BigCanvas::~BigCanvas() {
	// TODO Auto-generated destructor stub
}


inline void mergeIntoBGR(const Mat& r,const Mat& g,const Mat& b,Mat& output) {
	std::vector<cv::Mat> array_to_merge;

	array_to_merge.push_back(b);
	array_to_merge.push_back(g);
	array_to_merge.push_back(r);

	cv::merge(array_to_merge,output);
}




inline void getRoi(Rect& roi,const Point2f position,const Point2i bigCanvasSize,const Point2i smallCanvasSize) {

	int positionx = position.x*(bigCanvasSize.x);
	int positiony = position.y*(bigCanvasSize.y);

	positionx =
			(positionx < 0)? 0:
					(positionx >=(bigCanvasSize.x - smallCanvasSize.x))?
							(bigCanvasSize.x - smallCanvasSize.x): positionx;

	positiony =
			(positiony < 0)? 0:
					(positiony >=(bigCanvasSize.y - smallCanvasSize.y))?
							(bigCanvasSize.y - smallCanvasSize.y): positiony;

	roi = Rect(
			positionx,
			positiony,
			smallCanvasSize.x,
			smallCanvasSize.y
	);



}


void BigCanvas::getFrame(Mat& inputImage,Mat& outputImage) {

	// apply filter, get mask in _a,
	// and enhanced mask in _smallCanvas.
	_retinaFilter.applyFilter(inputImage,_alpha,_smallCanvas);


	// draw borders around the resultant image and the alpha channel.
	copyMakeBorder(_smallCanvas,_smallCanvasWithBorders,2,2,2,2,BORDER_CONSTANT,255);
	copyMakeBorder(_alpha,_alphaWithBorders,2,2,2,2,BORDER_CONSTANT,255);

	// assign shades to the r,g,b channels.
	_r = _smallCanvasWithBorders * _foregroundColor.x;
	_g = _smallCanvasWithBorders * _foregroundColor.y;
	_b = _smallCanvasWithBorders * _foregroundColor.z;

	// coloredSmallCanvas is a combination of _b,_g,_r
	Mat coloredSmallCanvas[3] = {_b,_g,_r};

	bitwise_not(_alphaWithBorders,_notA);


	// split the big canvas into its rgb components
	Mat bgr[3];
	split(_bigCanvas,bgr);

	// Get the area of the big canvas under the small canvas.
	Rect roi;
	getRoi(roi,_position,Point2i(_bigCanvas.cols,_bigCanvas.rows),Point2i(_smallCanvasWithBorders.cols,_smallCanvasWithBorders.rows));

	// iterate over the three channels b,g,r.
	for(int i=0;i<3;i++) {
		Mat ithChannel = bgr[i];

		Mat imageUnderRoi = ithChannel(roi);

		// AND the roi with the not mask,
		// doing this will clear the area where
		// _a is white.
		bitwise_and(imageUnderRoi,_notA,imageUnderRoi);

		// doing this will fill the areas where _a is white.
		bitwise_or(coloredSmallCanvas[i],imageUnderRoi,imageUnderRoi);


	}

	mergeIntoBGR(bgr[2],bgr[1],bgr[0],outputImage);
}


void BigCanvas::lock() {
	// save the current big canvas
	// to the undo stack. And clear the redo stack.
	Mat oldBigCanvas;
	_bigCanvas.copyTo(oldBigCanvas);
	_undoStack.push_back(oldBigCanvas);
	_redoStack.clear();


	// assign shades to the r,g,b channels.
	_r = _smallCanvas * _foregroundColor.x;
	_g = _smallCanvas * _foregroundColor.y;
	_b = _smallCanvas * _foregroundColor.z;

	// coloredSmallCanvas is a combination of _b,_g,_r
	Mat coloredSmallCanvas[3] = {_b,_g,_r};

	bitwise_not(_alpha,_notA);


	// split the big canvas into its rgb components
	Mat bgr[3];
	split(_bigCanvas,bgr);

	// Get the area of the big canvas under the small canvas.
	Rect roi;
	getRoi(roi,_position,Point2i(_bigCanvas.cols,_bigCanvas.rows),Point2i(_smallCanvas.cols,_smallCanvas.rows));

	// iterate over the three channels b,g,r.
	for(int i=0;i<3;i++) {
		Mat ithChannel = bgr[i];

		Mat imageUnderRoi = ithChannel(roi);

		// AND the roi with the not mask,
		// doing this will clear the area where
		// _a is white.
		bitwise_and(imageUnderRoi,_notA,imageUnderRoi);

		// doing this will fill the areas where _a is white.
		bitwise_or(coloredSmallCanvas[i],imageUnderRoi,imageUnderRoi);


	}

	mergeIntoBGR(bgr[2],bgr[1],bgr[0],_bigCanvas);
}


void BigCanvas::undo() {
	if(_undoStack.size() <= 0)
		return;

	// push the current bigCanvas to the redo stack.
	_redoStack.push_back(_bigCanvas);

	// pop the last canvas in the undo stack to the
	// bigCanvas.
	_bigCanvas = _undoStack.back();
	_undoStack.pop_back();
}

void BigCanvas::redo() {
	if(_redoStack.size() <= 0)
		return;
	_undoStack.push_back(_bigCanvas);

	_bigCanvas = _redoStack.back();
	_redoStack.pop_back();
}

void BigCanvas::setInkColor(Point3f color) {
	_foregroundColor = color;
}


void BigCanvas::setNoise(int val) {
	_retinaFilter.setNoiseSupressionLeveL(val);
}

void BigCanvas::setStrokeWidth(int val) {
	_retinaFilter.setAdaptiveKernelSize(val);
}



void BigCanvas::setBrightness(float percentBright) {
	_retinaFilter.setBrightness(percentBright * 255);
}

void  BigCanvas::exportAsImage(std::string path){
	imwrite(path.c_str(),_bigCanvas);
}



void BigCanvas::setWidth(float percentage) {
	_smallCanvasWidth = percentage;
	updateCanvasSize();
}

void BigCanvas::setHeight(float percentage) {
	_smallCanvasHeight = percentage;
	updateCanvasSize();
}

void BigCanvas::setPosition(Point2f normalizedPosition) {
	_position = normalizedPosition;
}






void BigCanvas::setPerspective(std::vector<Point2f> points) {
	if(points.size() != 4) {
		std::cout << "setPerspective requires a vector of length 4.";
		exit(0);
	}

	_retinaFilter.getPerspectiveCorrection()
			.setSurfaceCorners(points[0],points[1],points[2],points[3]);
}
