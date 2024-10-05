//
// Created by FabianKla on 05/10/2024.
//

#ifndef OPENCANCHANNE_H
#define OPENCANCHANNE_H

#include <iostream>
#include <napi.h>
#include <string>

// Function to open a CAN channel, set the bitrate, and go on-bus
canHandle openCanChannel(int channel, int bitrate) {
    // Open the CAN channel
    canHandle hnd = canOpenChannel(channel, canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
        std::cerr << "Error: Failed to open CAN channel!" << std::endl;
        return hnd; // Return the error handle
    }

    // Set the bitrate (e.g., 500Kbps)
    canStatus status = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    if (status != canOK) {
        std::cerr << "Error: Failed to set CAN bitrate!" << std::endl;
        canClose(hnd);
        return -1; // Return error code
    }

    // Go on-bus to start communication
    status = canBusOn(hnd);
    if (status != canOK) {
        std::cerr << "Error: Failed to go on-bus!" << std::endl;
        canClose(hnd);
        return -1; // Return error code
    }
    return hnd; // Return the handle if everything was successful
}


#endif //OPENCANCHANNE_H
