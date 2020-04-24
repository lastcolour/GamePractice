#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Platforms/Desktop/DesktopModule.hpp"
#include "Core/SystemModule.hpp"

std::unique_ptr<SystemModule> DesktopPlatform::createPlatformModule() const {
    return std::unique_ptr<SystemModule>(new DesktopModule);
}