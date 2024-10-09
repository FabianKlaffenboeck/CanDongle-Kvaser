//
// Created by FabianKla on 09/10/2024.
//

#ifndef LISTENCAN_H
#define LISTENCAN_H

#include "../Canlib/INC/canlib.h"
#include <napi.h>
#include "../../node_modules/node-addon-api/napi.h"

Napi::ThreadSafeFunction tsfn;

void startCanListener(canHandle hnd) {
    while (true) {
        long id;
        unsigned char data[8];
        unsigned int dataLength;
        unsigned int flags;
        unsigned long timestamp;

        canStatus status = canRead(hnd, &id, data, &dataLength, &flags, &timestamp);
        if (status == canOK) {
            tsfn.BlockingCall([id, data, dataLength](Napi::Env env, Napi::Function jsCallback) {
                Napi::Array dataArray = Napi::Array::New(env, dataLength);
                for (unsigned int i = 0; i < dataLength; i++) {
                    dataArray[i] = Napi::Number::New(env, data[i]);
                }

                jsCallback.Call({Napi::Number::New(env, id), dataArray, Napi::Number::New(env, dataLength)});
            });
        } else if (status != canERR_NOMSG) {
            std::cerr << "Error: Failed to read CAN message!" << std::endl;
        }
    }
}

#endif //LISTENCAN_H
