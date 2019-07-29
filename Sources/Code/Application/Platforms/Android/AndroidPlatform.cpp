#include "Platforms/Android/AndroidPlatform.hpp"

AndroidPlatform::AndroidPlatform(ANativeActivity* activity, void* savedState, size_t savedStateSize) {
}

AndroidPlatform::~AndroidPlatform() {
}

std::unique_ptr<SystemModule> AndroidPlatform::createPlatformModule() const {
    return nullptr;
}