#ifndef __ANDROIND_PLATFORM_HPP__
#define __ANDROIND_PLATFORM_HPP__

#include "Platform.hpp"

class AndroidPlatform : public Platform {
public:
    AndroidPlatform();
    virtual ~AndroidPlatform();

    // Platform
    std::unique_ptr<SystemModule> createPlatformModule() const override;
};

#endif /* __ANDROIND_PLATFORM_HPP__ */