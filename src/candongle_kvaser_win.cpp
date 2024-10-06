#include <cstdio>
#include <napi.h>
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

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "Expected the first argument to be a number (id)").
                ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[1].IsNumber()) {
        Napi::TypeError::New(env, "Expected the second argument to be a number (baudRate)").
                ThrowAsJavaScriptException();
        return env.Null();
    }

    int channel = info[0].As<Napi::Number>().Int32Value();
    int baudRate = info[1].As<Napi::Number>().Int32Value();

    // Log the extracted values for debugging
    std::cout << "Channel: " << channel << std::endl;
    std::cout << "BaudRate: " << baudRate << std::endl;

    // Call your CAN API to open the channel
    canHandle handle = openCanChannel(channel, baudRate); // Convert string channel to integer
    if (handle < 0) {
        Napi::Error::New(env, "Failed to open CAN channel").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Create a new CanDevice instance using the handle
    Napi::Object canDeviceInstance = CanDevice::NewInstance(handle, env);

    // // Check if there is a third argument and it's a function (callback)
    // if (info.Length() > 2 && info[2].IsFunction()) {
    //     Napi::Function callback = info[2].As<Napi::Function>();
    //
    //     // Call the callback with the newly created CanDevice instance
    //     callback.Call(env.Global(), {canDeviceInstance});
    // }

    return canDeviceInstance; // Return the instance in case no callback is passed
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    exports.Set("open", Napi::Function::New(env, OpenCanChannel));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
