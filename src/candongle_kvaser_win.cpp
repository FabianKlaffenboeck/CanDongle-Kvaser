#include <cstdio>
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"
#include <cstring>

#include "ListCanChannels.h"
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

Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
