//
// Created by FabianKla on 05/10/2024.
//

#ifndef CANDEVICE_H
#define CANDEVICE_H

#include <napi.h>
#include "../Canlib/INC/canlib.h"

class CanDevice : public Napi::ObjectWrap<CanDevice> {
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    static Napi::Object NewInstance(int handle, Napi::Env env); // Updated method to create instances

    CanDevice(const Napi::CallbackInfo &info);

    ~CanDevice();

private:
    static Napi::FunctionReference constructor;

    Napi::Value GetIsOpen(const Napi::CallbackInfo &info);

    Napi::Value Close(const Napi::CallbackInfo &info);

    Napi::Value Read(const Napi::CallbackInfo &info);

    Napi::Value SetMessageCallback(const Napi::CallbackInfo &info);

    Napi::Value Write(const Napi::CallbackInfo &info);

    int handle; // Store the handle for the CAN channel
    bool isOpen; // State of the CAN device
    Napi::FunctionReference *messageCallback; // Store the callback function reference
};


#endif //CANDEVICE_H
