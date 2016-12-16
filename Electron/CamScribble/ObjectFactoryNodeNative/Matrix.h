#include "OpenCV.h"

class Matrix: public Nan::ObjectWrap {
public:
  cv::Mat mat;
  Matrix(): Nan::ObjectWrap() {};

  static Nan::Persistent<FunctionTemplate> constructor;
  
  static void Init(Local<Object> target) {
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

  static NAN_METHOD(New) {
    Nan::HandleScope scope;
    if (info.This()->InternalFieldCount() == 0) {
      Nan::ThrowTypeError("Cannot instantiate without new");
    }

    Matrix *mat;

    mat = new Matrix();

    mat->Wrap(info.Holder());
    info.GetReturnValue().Set(info.Holder());
  }

  

  static NAN_METHOD(GetFrame) {
    Nan::HandleScope scope;
    Matrix *self = Nan::ObjectWrap::Unwrap<Matrix>(info.This());

    // calculate the size of the image.
    int sizeOfImage = self->mat.rows*self->mat.cols*3;
    
    if(info.Length() == 1) {
      // unwrap the passed buffer.
      // copy data from image to buffer.
      // return the buffer object.
      v8::Local < v8::Object > buf =  info[0]->ToObject();
      memcpy((uchar*) Buffer::Data(buf), self->mat.data, sizeOfImage);
      info.GetReturnValue().Set(buf);
      return;
    }
    

    // create a buffer to hold the image data.
    Local < Object > buf = Nan::NewBuffer(sizeOfImage).ToLocalChecked();

    // copy data from image to buffer.
    memcpy((uchar*) Buffer::Data(buf), self->mat.data, sizeOfImage);


    // get global context of app.
    v8::Local < v8::Object > globalObj = Nan::GetCurrentContext()->Global();

    // get handle to the node's Buffer object's constructor
    v8::Local < v8::Function > bufferConstructor =
      v8::Local<v8::Function>::Cast(globalObj->Get(Nan::New<v8::String>("Buffer").ToLocalChecked()));

    // create arguments for the buffer object.
    v8::Local<v8::Value> constructorArgs[3] = {buf, Nan::New<v8::Integer>((unsigned)sizeOfImage), Nan::New<v8::Integer>(0)};

    // instantiate the buffer object.
    v8::Local < v8::Object > actualBuffer = bufferConstructor->NewInstance(3,constructorArgs);

    // return the buffer object.
    info.GetReturnValue().Set(actualBuffer);
  }

  static NAN_METHOD(Width) {
    Nan::HandleScope scope;
    Matrix *self = Nan::ObjectWrap::Unwrap<Matrix>(info.This());

    info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().width));
  }

  static NAN_METHOD(Height) {
    Nan::HandleScope scope;
    Matrix *self = Nan::ObjectWrap::Unwrap<Matrix>(info.This());

    info.GetReturnValue().Set(Nan::New<Number>(self->mat.size().height));
  }
};