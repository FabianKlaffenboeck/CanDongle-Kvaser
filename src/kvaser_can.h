#ifndef CANHANDLER_H
#define CANHANDLER_H

#include "../Canlib/INC/canlib.h"
#include <iostream>
#include <stdexcept>
#include <vector>
#include <string>
#include <thread>
#include <functional>

// Exception class for CAN errors
class CanException : public std::runtime_error {
public:
    explicit CanException(const std::string& message);
};

// Main CanHandler class
class CanHandler {
private:
    canHandle handle;
    bool isOnBus;

    // Helper method to check for errors and throw exceptions if necessary
    void checkForError(const std::string& operation, canStatus status);

public:
    // Constructor and Destructor
    CanHandler();
    ~CanHandler();

    // Static function to list available CAN channels
    static std::vector<std::string> listChannels();

    // Method to open a CAN channel
    void openChannel(int channel, int bitrate);

    // Method to close a CAN channel
    void closeChannel();

    // Method to send a CAN frame
    void sendFrame(long id, const std::vector<unsigned char>& data);

    // Method to start a listener on a separate thread
    void startListener(std::function<void(long, const std::vector<unsigned char>&)> callback);
};

#endif // CANHANDLER_H
