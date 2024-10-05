#include <cstdio>
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"
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

    std::cout << "Number of arguments: " << info.Length() << std::endl;
    std::cout << "Argument 1 (Channel): Type - " << info[0].Type() << std::endl;
    std::cout << "Argument 2 (BaudRate): Type - " << info[1].Type() << std::endl;
    std::cout << "Argument 3 (BaudRate): Type - " << info[2].Type() << std::endl;


    if (info.Length() != 2 || !info[0].IsNumber() || !info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected id (number) and baudRate (number)").ThrowAsJavaScriptException();
        return env.Null();
    }

    const int id = info[0].As<Napi::Number>().Int32Value();
    const int baudRate = info[1].As<Napi::Number>().Int32Value();

    const canHandle handle = openCanChannel(id, baudRate);
    if (handle < 0) {
        Napi::Error::New(env, "Failed to open CAN channel").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object canDeviceInstance = CanDevice::NewInstance(handle, env);

    return canDeviceInstance;
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    exports.Set("open", Napi::Function::New(env, OpenCanChannel));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
