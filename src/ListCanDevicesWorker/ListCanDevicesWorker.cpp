//
// Created by FabianKla on 05/10/2024.
//

#include "ListCanDevicesWorker.h"

ListCanDevicesWorker::ListCanDevicesWorker(const Napi::Function &callback): Napi::AsyncWorker(callback) {
}

// Execute function (background thread)
void ListCanDevicesWorker::Execute() {
    try {
        adapters = ListChannels(); // Assume ListChannels is declared elsewhere
    } catch (const std::exception &e) {
        SetError(e.what());
    }
}

// OnOK function (called when Execute is successful)
void ListCanDevicesWorker::OnOK() {
    Napi::Env env = Env();
    Napi::HandleScope scope(env);

    Napi::Array jsArray = Napi::Array::New(env, adapters.size());

    for (size_t i = 0; i < adapters.size(); i++) {
        Napi::Object jsAdapter = Napi::Object::New(env);
        jsAdapter.Set("name", Napi::String::New(env, adapters[i].name));
        jsArray.Set(i, jsAdapter);
    }

    Callback().Call({env.Null(), jsArray});
}

// OnError function (called if Execute fails)
void ListCanDevicesWorker::OnError(const Napi::Error &e) {
    const Napi::Env env = Env();
    Napi::HandleScope scope(env);
    Callback().Call({e.Value()});
}
