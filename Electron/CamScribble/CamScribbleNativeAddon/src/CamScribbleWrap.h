#include "OpenCV.h"
#include "BigCanvas.h"

class CamScribbleWrap: public Nan::ObjectWrap {
public:
  cv::VideoCapture camera;
  
  cv::Mat cameraImage;
  cv::Mat outputImage;

  cv::Mat smallCanvasImage;
  
  BigCanvas canvas;
  int cameraIndex;


  static bool isCameraValid(int camId) {
    try {
      cv::VideoCapture temp = cv::VideoCapture(camId);
    } catch(cv::Exception& e) {
      return false;
    }
    return true;
  }

  static int countCameras()
  {
  	cv::VideoCapture temp;
  	int max = 10;
  	for (int i = 0; i < max; i++){
  		if(!isCameraValid(i))
        return i;
  	}
  	return max;
  }


  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);

  CamScribbleWrap(cv::Size canvasSize,cv::Scalar backgroundColor);

  static NAN_METHOD(SetPerspective);
  static NAN_METHOD(SetNoise);
  static NAN_METHOD(SetStrokeWidth);
  static NAN_METHOD(SetBrightness);
  static NAN_METHOD(SetHeight);
  static NAN_METHOD(SetWidth);
  static NAN_METHOD(SetInkColor);

  static NAN_METHOD(SetPosition);
  static NAN_METHOD(Lock);
  static NAN_METHOD(Undo);
  static NAN_METHOD(Redo);
  static NAN_METHOD(ExportAsImage);
  //
  static NAN_METHOD(GetNewFrame);
  static NAN_METHOD(GetFrame);
  static NAN_METHOD(GetSmallCanvas);

  static NAN_METHOD(GetAvailableCameras);
  static NAN_METHOD(SetCamera);
  static NAN_METHOD(ReleaseCam);

  static NAN_METHOD(GetNewCameraFrame);
  static NAN_METHOD(GetCameraFrame);

};
