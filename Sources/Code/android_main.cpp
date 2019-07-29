#include "android_main.hpp"

#include "Application.hpp"
#include "Platforms/Android/AndroidPlatform.hpp"

void ANativeActivity_onCreate(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
    Application app(std::unique_ptr<Platform>(new AndroidPlatform(activity, savedState, savedStateSize)));
    app.run();
}