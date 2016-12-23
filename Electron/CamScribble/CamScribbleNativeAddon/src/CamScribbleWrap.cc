#include "CamScribbleWrap.h"
#include "Matrix.h"
#include <vector>

Nan::Persistent<FunctionTemplate> CamScribbleWrap::constructor;

void CamScribbleWrap::Init(Local<Object> target) {
  Nan::HandleScope scope;

  Local<FunctionTemplate> ctor = Nan::New<FunctionTemplate>(CamScribbleWrap::New);
  constructor.Reset(ctor);
  ctor->InstanceTemplate()->SetInternalFieldCount(1);
  ctor->SetClassName(Nan::New("CamScribble").ToLocalChecked());



  Nan::SetPrototypeMethod(ctor,"setPerspective",SetPerspective);
  Nan::SetPrototypeMethod(ctor,"setNoise",SetNoise);
  Nan::SetPrototypeMethod(ctor,"setStrokeWidth",SetStrokeWidth);
  Nan::SetPrototypeMethod(ctor,"setBrightness",SetBrightness);
  Nan::SetPrototypeMethod(ctor,"setHeight",SetHeight);
  Nan::SetPrototypeMethod(ctor,"setWidth",SetWidth);
  Nan::SetPrototypeMethod(ctor,"setInkColor",SetInkColor);
  Nan::SetPrototypeMethod(ctor,"setPosition",SetPosition);
  Nan::SetPrototypeMethod(ctor,"lock",Lock);
  Nan::SetPrototypeMethod(ctor,"undo",Undo);
  Nan::SetPrototypeMethod(ctor,"redo",Redo);
  Nan::SetPrototypeMethod(ctor,"exportAsImage",ExportAsImage);

  Nan::SetPrototypeMethod(ctor,"setCamera",SetCamera);

  Nan::SetPrototypeMethod(ctor,"getFrame",GetFrame);

  Nan::SetPrototypeMethod(ctor,"getCameraFrame",GetCameraFrame);

  //Nan::SetPrototypeMethod(ctor,"getCameraFrameAsync",GetCameraFrameAsync);

  Nan::SetPrototypeMethod(ctor,"getCalibrationFrames",GetCalibrationFrames);


  Nan::SetPrototypeMethod(ctor,"getAvailableCameras",GetAvailableCameras);
  Nan::SetPrototypeMethod(ctor,"releaseCam",ReleaseCam);


  target->Set(Nan::New("CamScribble").ToLocalChecked(), ctor->GetFunction());
}

// var camScribble = new CamScribble([400,400],[0,0,0]);
NAN_METHOD(CamScribbleWrap::New) {
  Nan::HandleScope scope;

  if (info.This()->InternalFieldCount() == 0)
  return Nan::ThrowTypeError("Cannot Instantiate without new");

  if(info.Length() !=2 || !info[0]->IsArray() || !info[1]->IsArray()) {
    return Nan::ThrowError("Error! usage example: var cs = new CamScribble([400,400],[0,0,0])");
  }

  Handle<Array> canvasSize = Handle<Array>::Cast(info[0]);
  if(
      canvasSize->Length() != 2 ||
      !canvasSize->Get(0)->IsNumber() ||
      !canvasSize->Get(1)->IsNumber()
    ) {
    return Nan::ThrowError("Error! the first argument should have exactly two integers - width,height.");
  }

  Handle<Array> backgroundColor = Handle<Array>::Cast(info[1]);
  if(
    backgroundColor->Length() != 3 ||
    !backgroundColor->Get(0)->IsNumber() ||
    !backgroundColor->Get(1)->IsNumber() ||
    !backgroundColor->Get(2)->IsNumber()
  ) {
    return Nan::ThrowError("Error! the second argument should have exactly three integers - r,g,b.");
  }

  int width = canvasSize->Get(0)->IntegerValue();
  int height = canvasSize->Get(1)->IntegerValue();
  int r = backgroundColor->Get(0)->IntegerValue();
  int g = backgroundColor->Get(1)->IntegerValue();
  int b = backgroundColor->Get(2)->IntegerValue();


  CamScribbleWrap *v =
  	new CamScribbleWrap(cv::Size(width,height),cv::Scalar(r,g,b));

  v->Wrap(info.This());

  info.GetReturnValue().Set(info.This());
}

CamScribbleWrap::CamScribbleWrap(cv::Size canvasSize,cv::Scalar backgroundColor) {
  std::vector<Point2f> points;
  points.push_back(Point2f(0.0f,0.0f));
  points.push_back(Point2f(0.0f,0.0f));
  points.push_back(Point2f(0.0f,0.0f));
  points.push_back(Point2f(0.0f,0.0f));

  cameraIndex = -1;

  canvas = BigCanvas(canvasSize,backgroundColor,points);

  // initialize libuv mutex and condition variables.
  uv_mutex_init(&mutex);
  uv_cond_init(&cv);
}

CamScribbleWrap::~CamScribbleWrap() {

  // destroy libuv mutex and condition variables.
  uv_mutex_destroy(&mutex);
  uv_cond_destroy(&cv);
}

// give normalized co-ordinates, in the order:
// top-left,top-right,bottom-left,bottom-right.
// camScribbleObj.setPerspective([[0,0],[0.5,0],[0,0.5],[0.5,0.5]]);
NAN_METHOD(CamScribbleWrap::SetPerspective) {
  Nan::HandleScope scope;
  CamScribbleWrap *self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  // return an error, if the first argument is not an array of length 4.
  if(info.Length() != 1 || !info[0]->IsArray() || Handle<Array>::Cast(info[0])->Length() != 4) {
    return Nan::ThrowError("Error! the function takes in exactly one array of 4 arrays.\nExample: camScribbleObj.setPerspective([[0,0],[0.5,0],[0,0.5],[0.5,0.5]])");
  }

  // return if any of the 4 members are not an array
  Handle<Array> mainArray = Handle<Array>::Cast(info[0]);
  for(int i=0;i<3;i++) {
    if(!mainArray->Get(i)->IsArray()) {
      return Nan::ThrowError("Error! each member of the main array should be an array of two numbers between 0-1 inclusive.\nExample: camScribbleObj.setPerspective([[0,0],[0.5,0],[0,0.5],[0.5,0.5]])");
    }
  }

  // return if any of the arrays have length more than 2, or they have
  // non-numbers.
  for(int i=0;i<4;i++) {
    Handle<Array> temp = Handle<Array>::Cast(mainArray->Get(i));
    if(temp->Length() != 2 || !temp->Get(0)->IsNumber() || !temp->Get(1)->IsNumber())
      return Nan::ThrowError("Error! each member of the main array should be an array of two numbers between 0-1 inclusive.\nExample: camScribbleObj.setPerspective([[0,0],[0.5,0],[0,0.5],[0.5,0.5]])");
  }

  // You're safe to proceed
  Handle<Array> topLeft = Handle<Array>::Cast(mainArray->Get(0));
  Handle<Array> topRight = Handle<Array>::Cast(mainArray->Get(1));
  Handle<Array> bottomLeft = Handle<Array>::Cast(mainArray->Get(2));
  Handle<Array> bottomRight = Handle<Array>::Cast(mainArray->Get(3));


  std::vector<cv::Point2f> points;
  points.push_back(Point2f(topLeft->Get(0)->NumberValue(),topLeft->Get(1)->NumberValue()));
  points.push_back(Point2f(topRight->Get(0)->NumberValue(),topRight->Get(1)->NumberValue()));
  points.push_back(Point2f(bottomLeft->Get(0)->NumberValue(),bottomLeft->Get(1)->NumberValue()));
  points.push_back(Point2f(bottomRight->Get(0)->NumberValue(),bottomRight->Get(1)->NumberValue()));

  self->canvas.setPerspective(points);

}

NAN_METHOD(CamScribbleWrap::SetNoise) {
  Nan::HandleScope scope;
  CamScribbleWrap *self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Error! This function requires exactly one integer as an argument.");
  }

  self->canvas.setNoise(info[0]->IntegerValue());
}

NAN_METHOD(CamScribbleWrap::SetStrokeWidth) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Error! This function requires exactly one integer as an argument.");
  }

  self->canvas.setStrokeWidth(info[0]->IntegerValue());
}

NAN_METHOD(CamScribbleWrap::SetBrightness) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Error! This function requires exactly one float as an argument.");
  }

  self->canvas.setBrightness((float)info[0]->NumberValue());
}

NAN_METHOD(CamScribbleWrap::SetHeight) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Error! This function requires exactly one float as an argument.");
  }


  self->canvas.setHeight((float)info[0]->NumberValue());
}

NAN_METHOD(CamScribbleWrap::SetWidth) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 || !info[0]->IsNumber()) {
    return Nan::ThrowError("Error! This function requires exactly one float as an argument.");
  }


  self->canvas.setWidth((float)info[0]->NumberValue());
}

NAN_METHOD(CamScribbleWrap::SetInkColor) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(
    info.Length() != 3 ||
    !info[0]->IsNumber() ||
    !info[1]->IsNumber() ||
    !info[2]->IsNumber()
  ) {
    return Nan::ThrowError("Error! This function requires exactly three integers as an argument.");
  }

  int r = info[0]->IntegerValue();
  int g = info[1]->IntegerValue();
  int b = info[2]->IntegerValue();

  self->canvas.setInkColor(cv::Point3f(r/255.0,g/255.0,b/255.0));
}

NAN_METHOD(CamScribbleWrap::SetPosition) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(
    info.Length() != 2 ||
    !info[0]->IsNumber() ||
    !info[1]->IsNumber()) {
    return Nan::ThrowError("Error! This function takes in exactly two floats having values between 0-1 inclusive.");
  }


  float positionx = (float) info[0]->NumberValue();
  float positiony = (float) info[1]->NumberValue();


  self->canvas.setPosition(Point2f(positionx,positiony));
}

NAN_METHOD(CamScribbleWrap::Lock) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  self->canvas.lock();
}

NAN_METHOD(CamScribbleWrap::Undo) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  self->canvas.undo();
}

NAN_METHOD(CamScribbleWrap::Redo) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  self->canvas.redo();
}

NAN_METHOD(CamScribbleWrap::ExportAsImage) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 ||
    !info[0]->IsString()) {
      return Nan::ThrowError("Error! This function takes in exactly one string - the file path to save.");
  }

  std::string filePath = std::string(*Nan::Utf8String(info[0]->ToString()));

  self->canvas.exportAsImage(filePath);
}

NAN_METHOD(CamScribbleWrap::SetCamera) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(info.Length() != 1 ||
    !info[0]->IsNumber()) {
      return Nan::ThrowError("Error! This function takes in exactly one integer - the camera Id.");
  }

  uv_mutex_lock(&(self->mutex));
  {
    // if the camera is open release it.
    if(self->cameraIndex != -1) {
      self->camera.release();
      self->cameraIndex = -1;
    }

    int cameraId = info[0]->IntegerValue();

    try {
      self->camera = cv::VideoCapture(cameraId);
      self->cameraIndex = cameraId;

      // std::string message = "camera open";
      // return Nan::ThrowError(message.c_str());
    } catch(cv::Exception& e) {
      self->cameraIndex = -1;
      std::string message = "cannot open camera "+std::to_string(cameraId)+". It might be in use or not present.";

      uv_mutex_unlock(&(self->mutex));
      return Nan::ThrowError(message.c_str());
    }
  }
  uv_mutex_unlock(&(self->mutex));


}

NAN_METHOD(CamScribbleWrap::ReleaseCam) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  uv_mutex_lock(&(self->mutex));
  {
    if(self->cameraIndex != -1) {
      self->camera.release();
      self->cameraIndex = -1;
    }
  }
  uv_mutex_unlock(&(self->mutex));
}

NAN_METHOD(CamScribbleWrap::GetAvailableCameras) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  uv_mutex_lock(&(self->mutex));
  {
    std::vector<int> cameras = self->countCameras();
    //int count = self->countCameras();

    Local<Array> cameraList = Array::New(info.GetIsolate());

    for(unsigned int i=0;i< cameras.size();i++) {
      cameraList->Set(i,Integer::New(info.GetIsolate(),cameras[i]));
    }

    info.GetReturnValue().Set(cameraList);
  }
  uv_mutex_unlock(&(self->mutex));
}



NAN_METHOD(CamScribbleWrap::GetFrame) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(self->cameraIndex == -1) {
    info.GetReturnValue().Set(Null(info.GetIsolate()));
    return Nan::ThrowError("Error! cannot get frame without camera input. \nPlease call '.setCamera()' with an appropriate cam id before calling this function again.");
  }


  // read a frame from the camera
  self->camera.read(self->cameraImage);

  // pass it through the bigCanvas pipeline, and
  // convert it to RGB format.
  self->canvas.getFrame(self->cameraImage, self->outputImage);
  cv::cvtColor(self->outputImage,self->outputImage,CV_BGR2RGB);


  if(info.Length() != 1) {

    Local<Object> toReturn =
      Nan::New(Matrix::constructor)->GetFunction()->NewInstance();

    Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(toReturn);

    img->mat = self->outputImage;

    info.GetReturnValue().Set(toReturn);

    return;
  }

  Local<Object> passed = info[0]->ToObject();
  Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(passed);

  img->mat = self->outputImage;

  info.GetReturnValue().Set(passed);

}


NAN_METHOD(CamScribbleWrap::GetCameraFrame) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());

  if(self->cameraIndex == -1) {
    info.GetReturnValue().Set(Null(info.GetIsolate()));
    return Nan::ThrowError("Error! cannot get frame without camera input. \nPlease call '.setCamera()' with an appropriate cam id before calling this function again.");
  }

  // read a frame from the camera
  self->camera.read(self->cameraImage);

  // convert to RGB
  cv::cvtColor(self->cameraImage,self->cameraImage,CV_BGR2RGB);

  if(info.Length() != 1) {

    Local<Object> toReturn =
      Nan::New(Matrix::constructor)->GetFunction()->NewInstance();

    Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(toReturn);

    img->mat = self->cameraImage;

    info.GetReturnValue().Set(toReturn);

    return;
  }

  Local<Object> passed = info[0]->ToObject();
  Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(passed);

  img->mat = self->cameraImage;

  info.GetReturnValue().Set(passed);

}


// This class describes the thread that's created
// when a call to GetCalibrationFrames is made.
class GetClibrationFramesAsyncWorker: public Nan::AsyncWorker {
  public: GetClibrationFramesAsyncWorker(Nan::Callback *callback, CamScribbleWrap* self) :
      Nan::AsyncWorker(callback),
      self(self) {
  }

  public: ~GetClibrationFramesAsyncWorker() {
  }

  // Executed inside the worker-thread.
  // It is not safe to access V8, or V8 data structures
  // here, so everything we need for input and output
  // should go on `this`.
  public: void Execute() {

    uv_mutex_lock(&(self->mutex));
    {
      if(self->cameraIndex == -1) {
        this->success = false;
        uv_mutex_unlock(&(self->mutex));
        return;
      }

      try {
        // read a frame from the camera
        if(self->camera.read(self->cameraImage)) {
          self->canvas.getSmallCanvas(self->cameraImage, self->smallCanvasImage);

          // convert to RGB images
          cv::cvtColor(self->smallCanvasImage,self->smallCanvasImage,CV_BGR2RGB);
          cv::cvtColor(self->cameraImage,self->cameraImage,CV_BGR2RGB);

          this->success = true;
        } else {
          this->success = false;
        }
      } catch(cv::Exception& e) {
        this->success = false;
        uv_mutex_unlock(&(self->mutex));
        return;
      }
    }

    uv_mutex_unlock(&(self->mutex));
  }

  // Executed when the async work is complete
  // this function will be run inside the main event loop
  // so it is safe to use V8 again
  void HandleOKCallback() {
    Nan::HandleScope scope;

    if(!this->success) {
      return Nan::ThrowError("Error! cannot get frame without camera input. \nPlease call '.setCamera()' with an appropriate cam id before calling getCalibrationFrames() again.");
    }

    Local<Object> cameraImageToReturn= Nan::New(Matrix::constructor)->GetFunction()->NewInstance();
    Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(cameraImageToReturn);
    img->mat = self->cameraImage;


    Local<Object> perspectiveCorrectionImage= Nan::New(Matrix::constructor)->GetFunction()->NewInstance();
    img = Nan::ObjectWrap::Unwrap<Matrix>(perspectiveCorrectionImage);
    img->mat = self->smallCanvasImage;

    Local<Value> argv[] = {
      cameraImageToReturn,
      perspectiveCorrectionImage
    };

    Nan::TryCatch try_catch;
    callback->Call(2, argv);
    if (try_catch.HasCaught()) {
      Nan::FatalException(try_catch);
    }
  }

  private: CamScribbleWrap *self;
  private: bool success;

};

// NAN_METHOD(CamScribbleWrap::GetCameraFrameAsync) {
//   Nan::HandleScope scope;
//   CamScribbleWrap *self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());
//
//   REQ_FUN_ARG(0, cb);
//
//   Nan::Callback *callback = new Nan::Callback(cb.As<Function>());
//   Nan::AsyncQueueWorker(new GetCameraFrameAsyncWorker(callback, self));
//
//   return;
// }




NAN_METHOD(CamScribbleWrap::GetCalibrationFrames) {
  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());



  REQ_FUN_ARG(0, cb);

  Nan::Callback *callback = new Nan::Callback(cb.As<Function>());

  Nan::AsyncQueueWorker(new GetClibrationFramesAsyncWorker(callback, self));

  // return nothing;
}


NAN_METHOD(CamScribbleWrap::GetSmallCanvas) {


  Nan::HandleScope scope;
  CamScribbleWrap* self = Nan::ObjectWrap::Unwrap<CamScribbleWrap>(info.This());





  if(self->cameraIndex == -1) {
    info.GetReturnValue().Set(Null(info.GetIsolate()));
    return Nan::ThrowError("Error! cannot get frame without camera input. \nPlease call '.setCamera()' with an appropriate cam id before calling this function again.");
  }


  // read a frame from the camera
  self->camera.read(self->cameraImage);

  self->canvas.getSmallCanvas(self->cameraImage, self->smallCanvasImage);


  cv::cvtColor(self->smallCanvasImage,self->smallCanvasImage,CV_BGR2RGB);


  if(info.Length() != 1) {

    Local<Object> toReturn =
      Nan::New(Matrix::constructor)->GetFunction()->NewInstance();

    Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(toReturn);

    img->mat = self->smallCanvasImage;

    info.GetReturnValue().Set(toReturn);

    return;
  }

  Local<Object> passed = info[0]->ToObject();
  Matrix *img = Nan::ObjectWrap::Unwrap<Matrix>(passed);

  img->mat = self->smallCanvasImage;

  info.GetReturnValue().Set(passed);
}
