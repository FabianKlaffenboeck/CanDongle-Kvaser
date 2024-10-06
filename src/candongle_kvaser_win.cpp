#include <cstdio>
#include <cstring>
#include <list>

#include "ListCanWorker.h"
#include "OpenCanWorker.h"
#include "Canlib/INC/canlib.h"
#include "utils.h"
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"
#include "WriteCanWorker.h"

Napi::Value ListCan(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (info.Length() < 1 || !info[0].IsFunction()) {
        Napi::TypeError::New(env, "Callback function expected").ThrowAsJavaScriptException();
        return env.Null();
    }

    const auto callback = info[0].As<Napi::Function>();

    auto *worker = new ListCanWorker(callback);
    worker->Queue();

    return env.Undefined();
}

Napi::Value OpenCan(const Napi::CallbackInfo &info) {
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

    auto callback = info[2].As<Napi::Function>();
    auto worker = new OpenCanWorker(callback);
    worker->path = path;
    worker->baudRate = getIntFromObject(options, "baudRate");

    worker->Queue();

    return env.Undefined(); // Return the instance in case no callback is passed
}

Napi::Value CloseCan(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return env.Undefined(); // Return the instance in case no callback is passed
}

Napi::Value ReadCan(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return env.Undefined(); // Return the instance in case no callback is passed
}

Napi::Value SetCallBackCan(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    return env.Undefined(); // Return the instance in case no callback is passed
}

Napi::Value WriteCan(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    if (!info[0].IsNumber()) {
        Napi::TypeError::New(env, "First argument must be a number").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[1].IsArray()) {
        Napi::TypeError::New(env, "Second argument must be an array").ThrowAsJavaScriptException();
        return env.Null();
    }
    if (!info[2].IsFunction()) {
        Napi::TypeError::New(env, "Third argument must be a function").ThrowAsJavaScriptException();
        return env.Null();
    }

    int path = info[0].ToNumber().Int32Value();
    std::list<CanMessage> messages;

    auto napiArray = info[1].As<Napi::Array>();
    for (uint32_t i = 0; i < napiArray.Length(); ++i) {
        Napi::Value item = napiArray[i];

        if (!item.IsObject()) {
            Napi::TypeError::New(env, "Array elements must be objects").ThrowAsJavaScriptException();
            return env.Null();
        }
        auto obj = item.As<Napi::Object>();

        CanMessage canMsg{};
        canMsg.id = getIntFromObject(napiArray, "id");
        canMsg.dlc = getIntFromObject(napiArray, "dlc");

        auto dataArray = obj.Get("data").As<Napi::Array>();
        for (unsigned char &j: canMsg.data) {
            Napi::Value value = dataArray[j];
            if (!value.IsNumber()) {
                Napi::TypeError::New(env, "`data` array elements must be numbers").ThrowAsJavaScriptException();
                return env.Null();
            }
            j = value.As<Napi::Number>().Int32Value();
        }

        messages.push_back(canMsg);
    }

    auto callback = info[2].As<Napi::Function>();
    auto *worker = new WriteCanWorker(callback);
    worker->path = path;
    worker->messages = messages;

    worker->Queue();

    return env.Undefined(); // Return the instance in case no callback is passed
}


Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("list", Napi::Function::New(env, ListCan));
    exports.Set("open", Napi::Function::New(env, OpenCan));
    exports.Set("close", Napi::Function::New(env, CloseCan));
    exports.Set("read", Napi::Function::New(env, ReadCan));
    exports.Set("setMessageCallback", Napi::Function::New(env, SetCallBackCan));
    exports.Set("write", Napi::Function::New(env, WriteCan));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
