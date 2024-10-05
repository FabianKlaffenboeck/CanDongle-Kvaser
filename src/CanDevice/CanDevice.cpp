//
// Created by FabianKla on 05/10/2024.
//

#include "CanDevice.h"
#include <iostream>
// #include "../Canlib/INC/canlib.h"

Napi::FunctionReference CanDevice::constructor;

CanDevice::CanDevice(const Napi::CallbackInfo &info) : Napi::ObjectWrap<CanDevice>(info) {
    this->handle = info[0].As<Napi::Number>().Int32Value(); // Get the handle passed as the first argument
}

Napi::Object CanDevice::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "CanDevice", {
                                          InstanceMethod("read", &CanDevice::Read),
                                          InstanceMethod("write", &CanDevice::Write),
                                          InstanceMethod("close", &CanDevice::Close),
                                      });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct();

    exports.Set("CanDevice", func);
    return exports;
}

Napi::Object CanDevice::NewInstance(int handle, Napi::Env env) {
    Napi::Object obj = constructor.New({Napi::Number::New(env, handle)});
    return obj;
}

Napi::Value CanDevice::Read(const Napi::CallbackInfo &info) {
}

Napi::Value CanDevice::Write(const Napi::CallbackInfo &info) {
}

Napi::Value CanDevice::Close(const Napi::CallbackInfo &info) {
}
