#ifndef __DESKTOP_PLATFORM_HPP__
#define __DESKTOP_PLATFORM_HPP__

#include "Platform.hpp"

class DesktopPlatform : public Platform {
public:

    DesktopPlatform(int argc, char* argv[]) {}
    virtual ~DesktopPlatform() = default;

    std::unique_ptr<SystemModule> createPlatformModule() const override;
};

#endif /* __DESKTOP_PLATFORM_HPP__ */