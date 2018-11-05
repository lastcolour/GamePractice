#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"

DesktopPlatform::DesktopPlatform(int argc, char* argv[]) :
    surface(new GLFWSurface) {
}

DesktopPlatform::~DesktopPlatform() {
}

bool DesktopPlatform::init() {
    if (!surface || !surface->init()) {
        return false;
    }
    return true;
}

bool DesktopPlatform::shouldRun() {
    return surface->shouldRun();
}

void DesktopPlatform::update() {
    surface->update();
}