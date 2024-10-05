#include <cstdio>
// #include <napi.h>
#include "../node_modules/node-addon-api/napi.h"
#include <cstring>

#include "ListCanChannels.h"
#include "OpenCanChanne.h"
#include "CanDevice/CanDevice.h"
#include "Canlib/INC/canlib.h"

Napi::Value ListCanDevices(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "Callback function expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto callback = info[0].As<Napi::Function>();

    auto *worker = new ListCanDevicesWorker(callback);
    worker->Queue();

    return env.Undefined();
}

Napi::Value OpenCanChannel(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    // Check if the correct number of arguments is provided
    if (info.Length() < 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected channel number and baud rate").ThrowAsJavaScriptException();
        return env.Null();
    }

    int channel = info[0].As<Napi::Number>().Int32Value();
    int baudRate = info[1].As<Napi::Number>().Int32Value();

    // Open the CAN channel and get the handle
    int handle = openCanChannel(channel, baudRate);
    if (handle < 0) {
        Napi::TypeError::New(env, "Failed to open CAN channel").ThrowAsJavaScriptException();
        return env.Null();
    }
printf("openCanChannel returned %d\n", handle);
    // Napi::Object canDeviceInstance = CanDevice::NewInstance(handle, env);
    return env.Null();
    // return canDeviceInstance;
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    exports.Set("open", Napi::Function::New(env, OpenCanChannel));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
