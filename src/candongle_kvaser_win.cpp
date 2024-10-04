#include <cstdio>
#include <napi.h>
// #include "../node_modules/node-addon-api/napi.h"
#include <thread>
#include <cstring>

#include "CheckForError.h"
#include "ListCanChannels.h"
#include "Canlib/INC/canlib.h"  // Include your CAN library headers here

// Function to list all CAN devices
Napi::Array ListCanDevices(const Napi::CallbackInfo &info) {
    Napi::Env env = info.Env();

    // Call the native C++ function to get the list of channels
    std::vector<AdapterInfo> adapters = ListChannels();

    // Create a JavaScript array to hold the result
    Napi::Array jsArray = Napi::Array::New(env, adapters.size());

    // Populate the JavaScript array with objects
    for (size_t i = 0; i < adapters.size(); i++) {
        printf(adapters[i].name.c_str());
        Napi::Object jsAdapter = Napi::Object::New(env);
        jsAdapter.Set("name", Napi::String::New(env, adapters[i].name));
        jsArray.Set(i, jsAdapter);
    }

    return jsArray; // Return the array of adapter info objects
}

// Initialize the addon
Napi::Object Init(Napi::Env env, Napi::Object exports) {
    canInitializeLibrary();
    exports.Set("listCanDevices", Napi::Function::New(env, ListCanDevices));
    return exports;
}

NODE_API_MODULE(canAddon, Init)
