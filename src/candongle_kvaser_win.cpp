#include <cstdio>
#include <cstring>
#include "ListCanDevicesWorker.h"
#include "OpenCanWorker.h"
#include "Canlib/INC/canlib.h"
#include "utils.h"
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"

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
        Napi::TypeError::New(env, "First argument must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    int path = info[0].ToNumber().Int32Value();

    if (!info[1].IsObject()) {
        Napi::TypeError::New(env, "Second argument must be an object").ThrowAsJavaScriptException();
        return env.Null();
    }
    Napi::Object options = info[1].ToObject();

    if (!info[2].IsFunction()) {
        Napi::TypeError::New(env, "Third argument must be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Function callback = info[2].As<Napi::Function>();
    OpenCanWorker *worker = new OpenCanWorker(callback);
    worker->path = path;
    worker->baudRate = getIntFromObject(options, "baudRate");

    worker->Queue();

    return env.Undefined(); // Return the instance in case no callback is passed
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    exports.Set("open", Napi::Function::New(env, OpenCanChannel));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
