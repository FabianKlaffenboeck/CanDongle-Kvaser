#include "kvaser_can.h"

// CanException Constructor
CanException::CanException(const std::string& message) : std::runtime_error(message) {}

// CanHandler Constructor
CanHandler::CanHandler() : handle(-1), isOnBus(false) {}

// CanHandler Destructor
CanHandler::~CanHandler() {
    if (handle >= 0) {
        closeChannel();
    }
}

// Helper method to check for errors and throw exceptions if necessary
void CanHandler::checkForError(const std::string& operation, canStatus status) {
    if (status != canOK) {
        char errorText[255];
        canGetErrorText(status, errorText, sizeof(errorText));
        throw CanException(operation + " failed: " + errorText);
    }
}

// Static function to list available CAN channels
std::vector<std::string> CanHandler::listChannels() {
    std::vector<std::string> channels;
    int numberOfChannels;
    canStatus status = canGetNumberOfChannels(&numberOfChannels);

    if (status != canOK || numberOfChannels <= 0) {
        throw CanException("No CAN interfaces found");
    }

    for (int i = 0; i < numberOfChannels; ++i) {
        char deviceName[255];
        status = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, deviceName, sizeof(deviceName));
        if (status == canOK) {
            channels.emplace_back(deviceName);
        }
    }

    return channels;
}

// Method to open a CAN channel
void CanHandler::openChannel(int channel, int bitrate) {
    handle = canOpenChannel(channel, canOPEN_ACCEPT_VIRTUAL);
    if (handle < 0) {
        throw CanException("Failed to open CAN channel");
    }

    // Set bitrate
    checkForError("canSetBusParams", canSetBusParams(handle, bitrate, 0, 0, 0, 0, 0));

    // Go on bus
    checkForError("canBusOn", canBusOn(handle));
    isOnBus = true;
}

// Method to close a CAN channel
void CanHandler::closeChannel() {
    if (isOnBus) {
        canBusOff(handle);
    }
    if (handle >= 0) {
        canClose(handle);
        handle = -1;
    }
    isOnBus = false;
}

// Method to send a CAN frame
void CanHandler::sendFrame(long id, const std::vector<unsigned char>& data) {
    if (handle < 0) {
        throw CanException("CAN channel is not open");
    }

    canStatus status = canWrite(handle, id, const_cast<unsigned char*>(data.data()), data.size(), canMSG_STD);
    checkForError("canWrite", status);

    // Wait until the message is sent
    checkForError("canWriteSync", canWriteSync(handle, 1000));
}

// Method to start a listener on a separate thread
void CanHandler::startListener(std::function<void(long, const std::vector<unsigned char>&)> callback) {
    if (handle < 0) {
        throw CanException("CAN channel is not open");
    }

    std::thread([this, callback]() {
        while (true) {
            long id;
            unsigned char data[8];
            unsigned int dataLength;
            unsigned int flags;
            unsigned long timestamp;

            canStatus status = canRead(handle, &id, data, &dataLength, &flags, &timestamp);
            if (status == canOK) {
                // Pass the message to the callback
                callback(id, std::vector<unsigned char>(data, data + dataLength));
            } else if (status != canERR_NOMSG) {
                std::cerr << "Error: Failed to read CAN message!" << std::endl;
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(10));  // Avoid busy-waiting
        }
    }).detach();
}
