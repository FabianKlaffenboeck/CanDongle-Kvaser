//
// Created by FabianKla on 05/10/2024.
//

#ifndef LISTCANDEVICESWORKER_H
#define LISTCANDEVICESWORKER_H

#include <napi.h>

class ListCanDevicesWorker final : public Napi::AsyncWorker {
public:
    explicit ListCanDevicesWorker(const Napi::Function &callback): Napi::AsyncWorker(callback) {
    }

    void Execute() override {
        try {
            adapters = ListChannels();
        } catch (const std::exception &e) {
            SetError(e.what());
        }
    }

    void OnOK() override {
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

    // This code will be executed if there was an error in Execute
    void OnError(const Napi::Error &e) override {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Callback().Call({e.Value()});
    }

private:
    std::vector<AdapterInfo> adapters; // List of CAN devices
};


#endif //LISTCANDEVICESWORKER_H
