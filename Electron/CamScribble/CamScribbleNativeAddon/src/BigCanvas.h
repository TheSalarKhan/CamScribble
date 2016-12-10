/*
 * BigCanvas.h
 *
 *  Created on: Nov 22, 2016
 *      Author: salar
 */

#ifndef BIGCANVAS_H_
#define BIGCANVAS_H_

#include "OpenCV.h"
#include "RetinaFilter.h"

using namespace cv;

class BigCanvas {
public:
	BigCanvas();
	BigCanvas(Size sizeOfBigCanvas,Scalar backgroundColor,std::vector<Point2f> points);
	void getFrame(Mat& inputImage, Mat& outputImage);
	void undo();
	void redo();
	void setInkColor(Point3f color);
	void setNoise(int val);
	void setStrokeWidth(int val);
	void setPosition(Point2f normalizedPosition);
	void setBrightness(float percentBright);
	void exportAsImage(std::string path);
	void setWidth(float percentage);
	void setHeight(float percntage);
	void setPerspective(std::vector<Point2f> points);
	void lock();
	virtual ~BigCanvas();

private:

	void updateCanvasSize();

	// private vars
	Mat _bigCanvas;

	// This holds the direct output from
	// the retina filter
	Mat _smallCanvas;
	// This holds the _smallCanvas
	// after the border has been applied.
	Mat _smallCanvasWithBorders;
	// These are the components of the smaller canvas
	// used to form an RGBA image, that is pasted over
	// the larger backdrop.
	Mat _r,_g,_b,_alpha,_alphaWithBorders;

	// bitwise not of _alpha
	Mat _notA;


	Point3f _foregroundColor;

	// Retina filter and its settings
	RetinaFilter _retinaFilter;
	int _noise;
	int _strokeWidth;
	int _brightness;


	// settings for small canvas relative to big canvas.
	Point2f _position;
	float _smallCanvasWidth;
	float _smallCanvasHeight;

	std::vector<Mat> _undoStack,_redoStack;
};

#endif /* BIGCANVAS_H_ */
