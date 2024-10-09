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

        // Try to read a message from the CAN bus
        canStatus status = canRead(hnd, &id, data, &dataLength, &flags, &timestamp);
        if (status == canOK) {
            // Pass the message to the JavaScript callback using ThreadSafeFunction
            tsfn.BlockingCall([id, data, dataLength](Napi::Env env, Napi::Function jsCallback) {
                // Create a JS array to pass data bytes
                Napi::Array dataArray = Napi::Array::New(env, dataLength);
                for (unsigned int i = 0; i < dataLength; i++) {
                    dataArray[i] = Napi::Number::New(env, data[i]);
                }

                // Call the JS callback with `id`, `data[]`, and `dataLength`
                jsCallback.Call({Napi::Number::New(env, id), dataArray, Napi::Number::New(env, dataLength)});
            });
        } else if (status != canERR_NOMSG) {
            // Handle error (ignore canERR_NOMSG since it's not really an error)
            std::cerr << "Error: Failed to read CAN message!" << std::endl;
        }

        // Sleep to avoid busy-waiting (e.g., 10 ms)
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
}

#endif //LISTENCAN_H
