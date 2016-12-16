// myobject.h
#ifndef MYOBJECT_H
#define MYOBJECT_H

#include "includes.h"

namespace demo {

class MyObject : public node::ObjectWrap {
 public:
 	static Nan::Persistent<FunctionTemplate> constructor;
	static void Init(Local<Object> target);
	static NAN_METHOD(New);
	static NAN_METHOD(NewInstance);
	static NAN_METHOD(PlusOne);

	MyObject(double value = 0);
	~MyObject();

	double value_;
};

}  // namespace demo

#endif