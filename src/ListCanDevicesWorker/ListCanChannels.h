//
// Created by FabianKla on 04/10/2024.
//

#ifndef LISTCANCHANNELS_H
#define LISTCANCHANNELS_H
#include <cstdio>
#include <sstream>
#include <vector>
#include "../Canlib/INC/canlib.h"
#include "../CheckForError.h"
#include "../Interfaces.h"


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
    printf("Found %d channels\n", number_of_channels);

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


#endif //LISTCANCHANNELS_H
