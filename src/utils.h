//
// Created by FabianKla on 06/10/2024.
//

#ifndef UTILS_H
#define UTILS_H

#include <napi.h>

Napi::Value getValueFromObject(Napi::Object options, std::string key) {
    Napi::String str = Napi::String::New(options.Env(), key);
    return (options).Get(str);
}

int getIntFromObject(Napi::Object options, std::string key) {
    return getValueFromObject(options, key).ToNumber().Int64Value();
}

bool getBoolFromObject(Napi::Object options, std::string key) {
    return getValueFromObject(options, key).ToBoolean().Value();
}

Napi::String getStringFromObj(Napi::Object options, std::string key) {
    return getValueFromObject(options, key).ToString();
}

double getDoubleFromObject(Napi::Object options, std::string key) {
    return getValueFromObject(options, key).ToNumber().DoubleValue();
}

#endif //UTILS_H
