//
// Created by FabianKla on 05/10/2024.
//

#ifndef CLOSECANWORKER_H
#define CLOSECANWORKER_H

#include <iostream>
#include <string>
#include <napi.h>
#include "Canlib/INC/canlib.h"

canHandle closeCanChannel(const int channel) {
    if (canStatus stat = canClose(channel); stat < 0) {
        std::cerr << "Error: Close faild" << std::endl;
    }
    return canINVALID_HANDLE;
}

class CloseCanWorker final : public Napi::AsyncWorker {
public:
    explicit CloseCanWorker(const Napi::Function &callback): Napi::AsyncWorker(callback) {
    }

    void Execute() override {
        try {
            closeCanChannel(handle);
        } catch (const std::exception &e) {
            SetError(e.what());
        }
    }

    void OnOK() override {
        Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Callback().Call({env.Null(), Napi::Number::New(env, handle)});
    }

    void OnError(const Napi::Error &e) override {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Callback().Call({e.Value()});
    }

    int handle = -1;
};


#endif //OPENCANWORKER_H
