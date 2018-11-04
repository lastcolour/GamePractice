#ifndef __DESKTOP_PLATFORM_HPP__
#define __DESKTOP_PLATFORM_HPP__

#include "Platform.hpp"

class DesktopPlatform : public Platform {
public:

    DesktopPlatform(int argc, char* argv[]);
    ~DesktopPlatform();

    bool init() override;
};

#endif /* __DESKTOP_PLATFORM_HPP__ */