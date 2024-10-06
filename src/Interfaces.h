//
// Created by FabianKla on 04/10/2024.
//

#ifndef INTERFACES_H
#define INTERFACES_H

#include <cstdint>
#include <string>

struct AdapterInfo {
    std::string name;
};

struct CanMessage {
    int id;
    uint8_t data[8]; // CAN messages are typically 8 bytes long
    int dlc; // Data Length Code
};

struct OpenOptions {
    std::string path;          // Path to the CAN device
    int baudRate;              // Baud rate for CAN communication
};


#endif //INTERFACES_H
