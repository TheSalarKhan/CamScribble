#include "Contours.h"
#include "Matrix.h"
#include "OpenCV.h"
#include <string.h>
#include <nan.h>

Nan::Persistent<FunctionTemplate> Matrix::constructor;


void Matrix::Init(Local<Object> target) {
  Nan::HandleScope scope;

  //Class
  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(Matrix::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("Matrix").ToLocalChecked());

  // Prototype

  Nan::SetPrototypeMethod(ctor, "width", Width);
  Nan::SetPrototypeMethod(ctor, "height", Height);
  Nan::SetPrototypeMethod(ctor, "getFrame", GetFrame);
  
  target->Set(Nan::New("Matrix").ToLocalChecked(), ctor->GetFunction());
};

NAN_METHOD(Matrix::New) {
  Nan::HandleScope scope;
  if (info.This()->InternalFieldCount() == 0) {
    Nan::ThrowTypeError("Cannot instantiate without new");
  }

  Matrix *mat;

  mat = new Matrix;

  mat->Wrap(info.Holder());
  info.GetReturnValue().Set(info.Holder());
}

Matrix::Matrix() :
    node_opencv::Matrix() {
  mat = cv::Mat();
}

NAN_METHOD(Matrix::Width) {
  SETUP_FUNCTION(Matrix)

  info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().width));
}

NAN_METHOD(Matrix::Height) {
  SETUP_FUNCTION(Matrix)

  info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().height));
}
