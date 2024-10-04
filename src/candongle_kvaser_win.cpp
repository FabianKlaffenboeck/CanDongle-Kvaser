#include <cstdio>
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"
#include <cstring>

#include "CheckForError.h"
#include "ListCanChannels.h"
#include "Canlib/INC/canlib.h"  // Include your CAN library headers here

// N-API method that starts the async worker
Napi::Value ListCanDevices(const Napi::CallbackInfo& info) {
    Napi::Env env = info.Env();

    // Check if the callback function is provided
    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "Callback function expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Function callback = info[0].As<Napi::Function>();

    // Create a new instance of the worker and queue it for execution
    ListCanDevicesWorker* worker = new ListCanDevicesWorker(callback);
    worker->Queue();  // Queue the worker for asynchronous execution

    return env.Undefined();  // Return undefined immediately (async)
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCanDevices));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
