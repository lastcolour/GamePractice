#include "android_main.hpp"

#include "Application.hpp"
#include "Platforms/Android/AndroidPlatform.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"

void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    AndroindPlatformHandler* handler = new AndroindPlatformHandler(activity, savedState, savedStateSize);
    handler->start();
}