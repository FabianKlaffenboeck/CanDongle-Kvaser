//
// Created by FabianKla on 05/10/2024.
//

#ifndef LISTCANDEVICESWORKER_H
#define LISTCANDEVICESWORKER_H

#include <vector>
#include <napi.h>
#include "ListCanChannels.h"

class ListCanDevicesWorker final : public Napi::AsyncWorker {
public:
    explicit ListCanDevicesWorker(const Napi::Function &callback);

    void Execute() override;

    void OnOK() override;

    void OnError(const Napi::Error &e) override;

private:
    std::vector<AdapterInfo> adapters; // List of CAN devices
};


#endif //LISTCANDEVICESWORKER_H
