#include <cstdio>
// #include <napi.h>
#include "../node_modules/node-addon-api/napi.h"
#include <thread>
#include <vector>
#include <cstring>
#include "../Canlib/INC/canlib.h"  // Include your CAN library headers here

Napi::ThreadSafeFunction tsfn;

// Helper function for error checking
void CheckForError(const char *cmd, canStatus stat) {
    if (stat != canOK) {
        char buf[255];
        canGetErrorText(stat, buf, sizeof(buf));
        printf("[%s] %s: failed, stat=%d\n", cmd, buf, (int) stat);
        abort();
    }
}

// Function to list all CAN devices
Napi::Array ListCanDevices(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    int number_of_channels;
    canStatus stat = canGetNumberOfChannels(&number_of_channels);
    CheckForError("canGetNumberOfChannels", stat);

    Napi::Array devices = Napi::Array::New(env, number_of_channels);

    for (int i = 0; i < number_of_channels; i++) {
        char device_name[255];
        stat = canGetChannelData(i, canCHANNELDATA_DEVDESCR_ASCII, device_name, sizeof(device_name));
        CheckForError("canGetChannelData", stat);

        Napi::Object adapterInfo = Napi::Object::New(env);
        adapterInfo.Set("name", Napi::String::New(env, device_name));

        devices[i] = adapterInfo;
    }

    return devices;
}

// Function to open a CAN channel
Napi::Value OpenCanChannel(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    // Extract the OpenOptions from the input
    Napi::Object options = info[0].As<Napi::Object>();
    int channel = options.Get("path").As<Napi::Number>().Int32Value();
    int bitrate = options.Get("baudRate").As<Napi::Number>().Int32Value();

    // Open the CAN channel
    canHandle hnd = canOpenChannel(channel, canOPEN_ACCEPT_VIRTUAL);
    if (hnd < 0) {
        Napi::Error::New(env, "Failed to open CAN channel").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Set the bitrate for the CAN channel
    canStatus status = canSetBusParams(hnd, bitrate, 0, 0, 0, 0, 0);
    if (status != canOK) {
        Napi::Error::New(env, "Failed to set CAN bitrate").ThrowAsJavaScriptException();
        canClose(hnd);
        return env.Null();
    }

    // Go on-bus to start communication
    status = canBusOn(hnd);
    if (status != canOK) {
        Napi::Error::New(env, "Failed to go on-bus").ThrowAsJavaScriptException();
        canClose(hnd);
        return env.Null();
    }

    return Napi::Number::New(env, hnd);
}

// Function to close a CAN channel
Napi::Value CloseCanChannel(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    canHandle hnd = info[0].As<Napi::Number>().Int32Value();

    canBusOff(hnd);
    canClose(hnd);

    return env.Null();
}

// Function to read a CAN message
Napi::Value ReadCanMessage(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    canHandle hnd = info[0].As<Napi::Number>().Int32Value();

    long id;
    unsigned char data[8];
    unsigned int dataLength;
    unsigned int flags;
    unsigned long timestamp;

    canStatus status = canRead(hnd, &id, data, &dataLength, &flags, &timestamp);
    if (status != canOK && status != canERR_NOMSG) {
        Napi::Error::New(env, "Failed to read CAN message").ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::Object message = Napi::Object::New(env);
    message.Set("id", Napi::Number::New(env, id));
    message.Set("data", Napi::Buffer<unsigned char>::Copy(env, data, dataLength));
    message.Set("time", Napi::Number::New(env, timestamp));

    return message;
}

// Function to write a CAN message
Napi::Value WriteCanMessage(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    canHandle hnd = info[0].As<Napi::Number>().Int32Value();
    long id = info[1].As<Napi::Number>().Int64Value();
    Napi::Buffer<unsigned char> dataBuffer = info[2].As<Napi::Buffer<unsigned char> >();

    canStatus status = canWrite(hnd, id, dataBuffer.Data(), dataBuffer.Length(), canMSG_STD); // Assuming standard frame
    if (status != canOK) {
        Napi::Error::New(env, "Failed to send CAN frame").ThrowAsJavaScriptException();
        return env.Null();
    }

    // Wait until the message is sent
    status = canWriteSync(hnd, 1000); // Wait up to 1000ms
    if (status != canOK) {
        Napi::Error::New(env, "Failed to sync CAN write").ThrowAsJavaScriptException();
    }

    return env.Null();
}

// Thread to handle incoming CAN messages and invoke the callback
void CANListener(canHandle hnd) {
    while (true) {
        long id;
        unsigned char data[8];
        unsigned int dataLength;
        unsigned int flags;
        unsigned long timestamp;

        canStatus status = canRead(hnd, &id, data, &dataLength, &flags, &timestamp);
        if (status == canOK) {
            tsfn.BlockingCall([id, data, dataLength](Napi::Env env, Napi::Function jsCallback) {
                Napi::Array dataArray = Napi::Array::New(env, dataLength);
                for (int i = 0; i < dataLength; i++) {
                    dataArray[i] = Napi::Number::New(env, data[i]);
                }
                jsCallback.Call({Napi::Number::New(env, id), dataArray, Napi::Number::New(env, dataLength)});
            });
        }
    }
}

// Function to set a message callback
Napi::Value SetMessageCallback(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();
    canHandle hnd = info[0].As<Napi::Number>().Int32Value();
    Napi::Function callback = info[1].As<Napi::Function>();

    tsfn = Napi::ThreadSafeFunction::New(env, callback, "CANListener", 0, 1);

    std::thread listenerThread(CANListener, hnd);
    listenerThread.detach();

    return env.Null();
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    exports.Set("listCanDevices", Napi::Function::New(env, ListCanDevices));
    exports.Set("openCanChannel", Napi::Function::New(env, OpenCanChannel));
    exports.Set("closeCanChannel", Napi::Function::New(env, CloseCanChannel));
    exports.Set("readCanMessage", Napi::Function::New(env, ReadCanMessage));
    exports.Set("writeCanMessage", Napi::Function::New(env, WriteCanMessage));
    exports.Set("setMessageCallback", Napi::Function::New(env, SetMessageCallback));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
