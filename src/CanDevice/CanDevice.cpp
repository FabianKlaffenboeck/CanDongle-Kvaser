//
// Created by FabianKla on 05/10/2024.
//

#include "CanDevice.h"

#include <iostream>

#include "../Canlib/INC/canlib.h"

bool closeCanChannel(canHandle hnd) {
    canBusOff(hnd);
    canClose(hnd);
    return true;
}

Napi::FunctionReference CanDevice::constructor;

// Initialize the CanDevice class and export it
Napi::Object CanDevice::Init(Napi::Env env, Napi::Object exports) {
    Napi::Function func = DefineClass(env, "CanDevice", {
                                          InstanceMethod("close", &CanDevice::Close),
                                          InstanceMethod("read", &CanDevice::Read),
                                          InstanceMethod("setMessageCallback", &CanDevice::SetMessageCallback),
                                          InstanceMethod("write", &CanDevice::Write),
                                      });

    constructor = Napi::Persistent(func);
    constructor.SuppressDestruct(); // Prevent automatic destruction
    exports.Set("CanDevice", func);

    return exports;
}

// Static method to create an instance
Napi::Object CanDevice::NewInstance(int handle, Napi::Env env) {
    Napi::Object obj = constructor.New({Napi::Number::New(env, handle)});
    return obj;
}


// Constructor implementation
CanDevice::CanDevice(const Napi::CallbackInfo &info): Napi::ObjectWrap<CanDevice>(info), handle(-1), isOpen(false), messageCallback(nullptr) {
    if (info.Length() > 0 && info[0].IsNumber()) {
        handle = info[0].As<Napi::Number>().Int32Value();
        isOpen = true; // Set isOpen to true as we have a valid handle
    }
}

// Destructor implementation
CanDevice::~CanDevice() {
    // Ensure the channel is closed when the object is destroyed
    if (isOpen) {
        closeCanChannel(handle);
    }
}

// GetIsOpen implementation
Napi::Value CanDevice::GetIsOpen(const Napi::CallbackInfo &info) {
    return Napi::Boolean::New(info.Env(), isOpen);
}

// Close implementation
Napi::Value CanDevice::Close(const Napi::CallbackInfo &info) {
    if (isOpen) {
        closeCanChannel(handle);
        handle = -1; // Invalidate handle
        isOpen = false; // Update state
    }
    return info.Env().Undefined();
}

// Read implementation
Napi::Value CanDevice::Read(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    long id;
    unsigned char data[8];
    unsigned int dataLength;
    unsigned int flags;
    unsigned long timestamp;

    // Read message from the CAN bus
    canStatus status = canRead(handle, &id, data, &dataLength, &flags, &timestamp);
    if (status == canOK) {
        Napi::Object message = Napi::Object::New(env);
        message.Set("id", Napi::Number::New(env, id));
        message.Set("data", Napi::Array::New(env, dataLength));

        for (unsigned int i = 0; i < dataLength; i++) {
            message.Get("data").As<Napi::Array>().Set(i, Napi::Number::New(env, data[i]));
        }

        message.Set("time", Napi::Number::New(env, timestamp));
        return message;
    } else if (status != canERR_NOMSG) {
        Napi::TypeError::New(env, "Failed to read CAN message").ThrowAsJavaScriptException();
    }
    return env.Null();
}

// SetMessageCallback implementation
Napi::Value CanDevice::SetMessageCallback(const Napi::CallbackInfo &info) {
    if (!info[0].IsFunction()) {
        Napi::TypeError::New(info.Env(), "Expected a callback function").ThrowAsJavaScriptException();
        return info.Env().Undefined();
    }

    // Store the callback
    messageCallback = new Napi::FunctionReference();
    *messageCallback = Napi::Persistent(info[0].As<Napi::Function>());
    return info.Env().Undefined();
}

// Write implementation
Napi::Value CanDevice::Write(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsArray()) {
        Napi::TypeError::New(env, "Expected an array of numbers").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Array dataArray = info[0].As<Napi::Array>();
    unsigned char data[8];
    size_t dataLength = dataArray.Length();

    if (dataLength > 8) {
        Napi::TypeError::New(env, "Data length must not exceed 8 bytes").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Convert Napi array to C++ array
    for (size_t i = 0; i < dataLength; i++) {
        data[i] = static_cast<unsigned char>(dataArray.Get(i).As<Napi::Number>().Int32Value());
    }

    // Send the CAN frame (example ID used)
    canStatus status = canWrite(handle, 0x123, data, dataLength, canMSG_STD);
    if (status != canOK) {
        Napi::TypeError::New(env, "Failed to send CAN frame").ThrowAsJavaScriptException();
    }
    return env.Undefined();
}
