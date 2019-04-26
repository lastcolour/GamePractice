#include "Application.hpp"
#include "Platform.hpp"
#include "CoreModule.hpp"
#include "Render/RenderModule.hpp"
#include "Game/GameModule.hpp"

namespace {
    const int APP_SUCCESSED = 0;
    const int APP_FAILED = -1;
} // namespace

Application::Application(std::unique_ptr<Platform>&& platform) {
    systemModules.emplace_back(new CoreModule);
    systemModules.emplace_back(platform ? platform->createPlatformModule() : nullptr);
    systemModules.emplace_back(new RenderModule);
    systemModules.emplace_back(new GameModule);
}

bool Application::init() {
    for(const auto& module : systemModules) {
        if(!module || !module->init()) {
            return false;
        }
    }
    return true;
}

int Application::run() {
    if(!init()) {
        return APP_FAILED;
    }
    mainLoop();
    return APP_SUCCESSED;
}

void Application::mainLoop() {
    while(true) {
    }
}