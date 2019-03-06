#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Platforms/Desktop/GLFWSurface.hpp"
#include "Platforms/Desktop/DesktopLogger.hpp"
#include "Platforms/Desktop/DesktopAssets.hpp"

DesktopPlatform::DesktopPlatform(int argc, char* argv[]) {
}

DesktopPlatform::~DesktopPlatform() {
}

bool DesktopPlatform::init() {
    return true;
}

std::unique_ptr<Surface> DesktopPlatform::createSurface() {
    return std::unique_ptr<Surface>(new GLFWSurface);
}

std::unique_ptr<Logger> DesktopPlatform::createLogger() {
    return std::unique_ptr<Logger>(new DesktopLogger);
}

std::unique_ptr<Assets> DesktopPlatform::createAssets() {
    return std::unique_ptr<Assets>(new DesktopAssets);
}