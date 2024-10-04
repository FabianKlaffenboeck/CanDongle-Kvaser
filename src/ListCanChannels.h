//
// Created by FabianKla on 04/10/2024.
//

#ifndef LISTCANCHANNELS_H
#define LISTCANCHANNELS_H
#include <cstdio>
#include <sstream>
#include <vector>

#include "CheckForError.h"
#include "interfaces.h"
#include "Canlib/INC/canlib.h"

// C++ Function to list CAN channels and return a vector of AdapterInfo
std::vector<AdapterInfo> ListChannels() {
    std::vector<AdapterInfo> adapters;

    canStatus stat;
    int number_of_channels;
    int device_channel;

    // Get number of channels
    stat = canGetNumberOfChannels(&number_of_channels);
    CheckForError("canGetNumberOfChannels", stat);

    if (number_of_channels > 0) {
        printf("Found %d channels\n", number_of_channels);
    } else {
        printf("Could not find any CAN interface.\n");
        return adapters; // Return an empty list if no channels found
    }

    for (int i = 0; i < number_of_channels; i++) {
        char device_name[255];
        stat = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
        CheckForError((char *) "canGetChannelData", stat);
        stat = canGetChannelData(i, canCHANNELDATA_CHAN_NO_ON_CARD, &device_channel, sizeof(device_channel));
        CheckForError((char *) "canGetChannelData", stat);
        std::stringstream ss;
        ss << "Found channel: " << i << " " << device_name << " " << (device_channel + 1);
        AdapterInfo info;
        info.name = std::string(ss.str());
        adapters.push_back(info);
    }

    return adapters;
}

#endif //LISTCANCHANNELS_H
