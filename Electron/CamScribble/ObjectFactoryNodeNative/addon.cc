// addon.cc
#include "includes.h"
#include "myobject.h"

namespace demo {


extern "C" void init(Local<Object> target) {
  Nan::HandleScope scope;


  MyObject::Init(target);

};

NODE_MODULE(cam_scribble, init)

// using v8::FunctionCallbackInfo;
// using v8::Isolate;
// using v8::Local;
// using v8::Object;
// using v8::String;
// using v8::Value;

// void CreateObject(const FunctionCallbackInfo<Value>& args) {
//   MyObject::NewInstance(args);
// }

// void InitAll(Local<Object> exports, Local<Object> module) {
//   MyObject::Init(exports->GetIsolate());

//   NODE_SET_METHOD(module, "exports", CreateObject);
// }

// NODE_MODULE(addon, InitAll)

}  // namespace demo
