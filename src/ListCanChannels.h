//
// Created by FabianKla on 04/10/2024.
//

#ifndef LISTCANCHANNELS_H
#define LISTCANCHANNELS_H
#include <cstdio>
#include <vector>

#include "CheckForError.h"
#include "interfaces.h"
#include "Canlib/INC/canlib.h"

// C++ Function to list CAN channels and return a vector of AdapterInfo
std::vector<AdapterInfo> ListChannels() {
    std::vector<AdapterInfo> adapters;
    int number_of_channels;

    // Get number of channels
    canStatus stat = canGetNumberOfChannels(&number_of_channels);
    CheckForError("canGetNumberOfChannels", stat);

    if (number_of_channels > 0) {
        printf("Found %d channels\n", number_of_channels);
    } else {
        printf("Could not find any CAN interface.\n");
        return adapters; // Return an empty list if no channels found
    }

    // Loop and print all channels
    for (int i = 0; i < number_of_channels; i++) {
        char device_name[255];
        stat = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
        CheckForError("canGetChannelData", stat);

        // Add the channel info to the list of adapters
        AdapterInfo info;
        info.name = std::string(device_name);
        adapters.push_back(info);
    }

    return adapters;
}

#endif //LISTCANCHANNELS_H
