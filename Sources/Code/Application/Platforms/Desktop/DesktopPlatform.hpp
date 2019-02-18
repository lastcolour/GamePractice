#ifndef __DESKTOP_PLATFORM_HPP__
#define __DESKTOP_PLATFORM_HPP__

#include "Platform.hpp"

class DesktopPlatform : public Platform {
public:

    DesktopPlatform(int argc, char* argv[]);
    virtual ~DesktopPlatform();

    bool init() override;

    std::unique_ptr<Surface> createSurface() override;
    std::unique_ptr<LogStream> createLogStream() override;
    std::unique_ptr<Assets> createAssets() override;
};

#endif /* __DESKTOP_PLATFORM_HPP__ */