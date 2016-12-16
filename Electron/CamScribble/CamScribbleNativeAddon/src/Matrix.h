#ifndef _MATRIX_H_
#define _MATRIX_H_

#include "OpenCV.h"

class Matrix: public Nan::ObjectWrap {
public:
  cv::Mat mat;
  Matrix(): Nan::ObjectWrap() {};

  static Nan::Persistent<FunctionTemplate> constructor;
  
  static void Init(Local<Object> target);

  static NAN_METHOD(New);

  static NAN_METHOD(ToBuffer);
  static NAN_METHOD(Width);
  static NAN_METHOD(Height);
};



#endif