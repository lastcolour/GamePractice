#include "Platforms/Android/AndroidPlatform.hpp"
#include "Platforms/Android/AndroidModule.hpp"

AndroidPlatform::AndroidPlatform() {
}

AndroidPlatform::~AndroidPlatform() {
}

std::unique_ptr<SystemModule> AndroidPlatform::createPlatformModule() const {
    return std::unique_ptr<SystemModule>(new AndroidModule());
}