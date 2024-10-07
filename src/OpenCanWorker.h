//
// Created by FabianKla on 05/10/2024.
//

#ifndef OPENCANWORKER_H
#define OPENCANWORKER_H

#include <iostream>
#include <string>
#include "Canlib/INC/canlib.h"
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"

canHandle openCanChannel(const int channel, const int bitrate) {
    const canHandle hnd = canOpenChannel(channel, canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
        std::cerr << "Error: Failed to open CAN channel!" << std::endl;
        return hnd;
    }

    canStatus status = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    if (status != canOK) {
        std::cerr << "Error: Failed to set CAN bitrate!" << std::endl;
        canClose(hnd);
        return -1;
    }

    // Go on-bus to start communication
    status = canBusOn(hnd);
    if (status != canOK) {
        std::cerr << "Error: Failed to go on-bus!" << std::endl;
        canClose(hnd);
        return -1;
    }
    return hnd;
}

class OpenCanWorker final : public Napi::AsyncWorker {
public:
    explicit OpenCanWorker(const Napi::Function &callback): Napi::AsyncWorker(callback) {
    }

    void Execute() override {
        try {
           handle = openCanChannel(path, baudRate);
           if(handle == -1){
             SetError("Somethink didn`t worke");
           }
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

    int path = -1;
    int handle = -1;
    int baudRate = -1;
};


#endif //OPENCANWORKER_H
