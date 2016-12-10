#include "VideoCaptureWrap.h"
#include "OpenCV.h"
#include  <iostream>


Nan::Persistent<FunctionTemplate> VideoCaptureWrap::constructor;


void VideoCaptureWrap::Init(Local<Object> target) {
  Nan::HandleScope scope;

  //Class
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(VideoCaptureWrap::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("VideoCapture").ToLocalChecked());


  Nan::SetPrototypeMethod(ctor, "read", Read);
  Nan::SetPrototypeMethod(ctor, "readIntoBuffer", ReadIntoBuffer);
  Nan::SetPrototypeMethod(ctor, "release", Release);

  target->Set(Nan::New("VideoCapture").ToLocalChecked(), ctor->GetFunction());
}

NAN_METHOD(VideoCaptureWrap::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0)
  return Nan::ThrowTypeError("Cannot Instantiate without new");

  VideoCaptureWrap *v;

  if (info[0]->IsNumber()) {
    v = new VideoCaptureWrap(info[0]->NumberValue());
  } else {
    //TODO - assumes that we have string, verify
    v = new VideoCaptureWrap(std::string(*Nan::Utf8String(info[0]->ToString())));
  }

  v->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

VideoCaptureWrap::VideoCaptureWrap(int device) {
  Nan::HandleScope scope;
  cap.open(device);

  if(!cap.isOpened()) {
    Nan::ThrowError("Camera could not be opened");
  }
}

VideoCaptureWrap::VideoCaptureWrap(const std::string& filename) {
  Nan::HandleScope scope;
  cap.open(filename);
  // TODO! At the moment this only takes a full path - do relative too.
  if(!cap.isOpened()) {
    Nan::ThrowError("Video file could not be opened (opencv reqs. non relative paths)");
  }
}


NAN_METHOD(VideoCaptureWrap::Release) {
  Nan::HandleScope scope;
  VideoCaptureWrap *v = Nan::ObjectWrap::Unwrap<VideoCaptureWrap>(info.This());

  v->cap.release();
}




NAN_METHOD(VideoCaptureWrap::Read) {

  Nan::HandleScope scope;
  VideoCaptureWrap *self = Nan::ObjectWrap::Unwrap<VideoCaptureWrap>(info.This());

  self->cap.read(self->mat);

  cv::cvtColor(self->mat,self->mat,CV_BGR2RGB);

  // calculate the size of the image.
  int sizeOfImage = self->mat.rows*self->mat.cols*3;

  // create a buffer to hold the image data.
  Local < Object > buf = Nan::NewBuffer(sizeOfImage).ToLocalChecked();

  // copy data from image to buffer.
  memcpy((uchar*) Buffer::Data(buf), self->mat.data, sizeOfImage);


  // get global context of app.
  v8::Local < v8::Object > globalObj = Nan::GetCurrentContext()->Global();

  // get handle to the node's Buffer object's constructor
  v8::Local < v8::Function > bufferConstructor = v8::Local<v8::Function>::Cast(globalObj->Get(Nan::New<String>("Buffer").ToLocalChecked()));

  // create arguments for the buffer object.
  v8::Local<v8::Value> constructorArgs[3] = {buf, Nan::New<v8::Integer>((unsigned)sizeOfImage), Nan::New<v8::Integer>(0)};

  // instantiate the buffer object.
  v8::Local < v8::Object > actualBuffer = bufferConstructor->NewInstance(3,constructorArgs);

  // return the buffer object.
  info.GetReturnValue().Set(actualBuffer);

}


NAN_METHOD(VideoCaptureWrap::ReadIntoBuffer) {

  Nan::HandleScope scope;

  // unwrap the object from the isolate.
  VideoCaptureWrap *self = Nan::ObjectWrap::Unwrap<VideoCaptureWrap>(info.This());

  // read frame
  self->cap.read(self->mat);

  // convert frame to RGB
  cv::cvtColor(self->mat,self->mat,CV_BGR2RGB);

  // calculate the size of the image.
  int sizeOfImage = self->mat.rows*self->mat.cols*3;

  // unwrap the passed buffer.
  v8::Local < v8::Object > buf =  info[0]->ToObject();//Nan::ObjectWrap::Unwrap< v8::Uint8Array >(info[0]->ToObject());//Nan::NewBuffer(sizeOfImage).ToLocalChecked();

  // copy data from image to buffer.
  memcpy((uchar*) Buffer::Data(buf), self->mat.data, sizeOfImage);


  // return the buffer object.
  info.GetReturnValue().Set(buf);

}
