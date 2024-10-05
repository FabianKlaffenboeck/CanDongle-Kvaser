//
// Created by FabianKla on 04/10/2024.
//

#ifndef LISTCANCHANNELS_H
#define LISTCANCHANNELS_H
#include <cstdio>
#include <sstream>
#include <vector>

#include "CheckForError.h"
#include "Interfaces.h"
#include "Canlib/INC/canlib.h"

std::vector<AdapterInfo> ListChannels() {
    std::vector<AdapterInfo> adapters;
    int number_of_channels;
    int device_channel;

    // Get number of channels
    canStatus stat = canGetNumberOfChannels(&number_of_channels);
    CheckForError("canGetNumberOfChannels", stat);

    if (number_of_channels <= 0) {
        printf("Could not find any CAN interface.\n");
        return adapters;
    }
    // printf("Found %d channels\n", number_of_channels);

    for (int i = 0; i < number_of_channels; i++) {
        char device_name[255];
        stat = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
        CheckForError("canGetChannelData", stat);

        stat = canGetChannelData(i, canCHANNELDATA_CHAN_NO_ON_CARD, &device_channel, sizeof(device_channel));
        CheckForError("canGetChannelData", stat);

        AdapterInfo info;
        info.name = std::string(device_name); // Channel name
        adapters.push_back(info);
    }

    return adapters;
}

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

#endif //LISTCANCHANNELS_H
