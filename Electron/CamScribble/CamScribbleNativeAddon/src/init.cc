#include "OpenCV.h"
#include "VideoCaptureWrap.h"
#include "CamScribbleWrap.h"

extern "C" void init(Local<Object> target) {
  Nan::HandleScope scope;


  VideoCaptureWrap::Init(target);
  CamScribbleWrap::Init(target);

};

NODE_MODULE(cam_scribble, init)
