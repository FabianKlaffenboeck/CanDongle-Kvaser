//
// Created by FabianKla on 05/10/2024.
//

#ifndef CANDEVICE_H
#define CANDEVICE_H

#include <napi.h>
// #include "../../node_modules/node-addon-api/napi.h"
#include "../Canlib/INC/canlib.h"

class CanDevice : public Napi::ObjectWrap<CanDevice> {
public:
    CanDevice(const Napi::CallbackInfo &info);

    static Napi::Object Init(Napi::Env env, Napi::Object exports);

    static Napi::Object NewInstance(int handle, Napi::Env env);

private:
    int handle;

    static Napi::FunctionReference constructor; // Declare static constructor

    Napi::Value Read(const Napi::CallbackInfo &info);

    Napi::Value Write(const Napi::CallbackInfo &info);

    Napi::Value Close(const Napi::CallbackInfo &info);
};


#endif //CANDEVICE_H
