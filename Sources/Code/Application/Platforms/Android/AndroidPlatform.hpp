#ifndef __ANDROIND_PLATFORM_HPP__
#define __ANDROIND_PLATFORM_HPP__

#include "Platform.hpp"

struct ANativeActivity;

class AndroidPlatform : public Platform {
public:
    AndroidPlatform(ANativeActivity* activity, void* savedState, size_t savedStateSize);
    virtual ~AndroidPlatform();

    // Platform
    std::unique_ptr<SystemModule> createPlatformModule() const override;
};

#endif /* __ANDROIND_PLATFORM_HPP__ */