//
// Created by FabianKla on 05/10/2024.
//

#ifndef WRITECANWORKER_H
#define WRITECANWORKER_H

#include <iostream>
#include <list>
#include "Canlib/INC/canlib.h"
#include "Interfaces.h"
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"


void sendCanFrame(canHandle hnd, long id, unsigned char *data, int dataLength) {

    canStatus status = canWrite(hnd, id, data, dataLength, canMSG_STD); // Use `canMSG_EXT` for extended frames
    if (status != canOK) {
        std::cerr << "Error: Failed to send CAN frame!" << std::endl;
        return;
    }

    status = canWriteSync(hnd, 1000);
    if (status != canOK) {
        std::cerr << "Error: Failed to sync CAN write!" << std::endl;
    }
}

class WriteCanWorker final : public Napi::AsyncWorker {
public:
    explicit WriteCanWorker(const Napi::Function &callback): Napi::AsyncWorker(callback) {
    }

    void Execute() override {
        try {
            for (auto message: messages) {
                sendCanFrame(path, message.id, message.data, message.dlc);
            }
        } catch (const std::exception &e) {
            SetError(e.what());
        }
    }

    void OnOK() override {
        Napi::Env env = Env();

        Callback().Call({env.Null(), env.Null()});
    }

    void OnError(const Napi::Error &e) override {
        const Napi::Env env = Env();
        Napi::HandleScope scope(env);
        Callback().Call({e.Value()});
    }

    int path;
    std::list<CanMessage> messages;
};


#endif //WRITECANWORKER_H
