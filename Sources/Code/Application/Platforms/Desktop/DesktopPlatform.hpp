#ifndef __DESKTOP_PLATFORM_HPP__
#define __DESKTOP_PLATFORM_HPP__

#include <memory>

#include "Platform.hpp"

class GLFWSurface;

class DesktopPlatform : public Platform {
public:

    DesktopPlatform(int argc, char* argv[]);
    ~DesktopPlatform();

    bool init() override;
    bool shouldRun() override;
    void update() override;

private:

    std::unique_ptr<GLFWSurface> surface;
};

#endif /* __DESKTOP_PLATFORM_HPP__ */