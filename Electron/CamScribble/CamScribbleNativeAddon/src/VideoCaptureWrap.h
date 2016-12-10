#include "OpenCV.h"
#include <time.h>

class VideoCaptureWrap: public Nan::ObjectWrap {
public:
  cv::VideoCapture cap;
  cv::Mat mat;

  static Nan::Persistent<FunctionTemplate> constructor;
  static void Init(Local<Object> target);
  static NAN_METHOD(New);

  VideoCaptureWrap(const std::string& filename);
  VideoCaptureWrap(int device);

  // read image from the camera,
  // synchronously. This blocks.
  static NAN_METHOD(Read);

  // release the stream
  static NAN_METHOD(Release);

  // read a frame into the passed buffer
  // object.
  static NAN_METHOD(ReadIntoBuffer);

};
