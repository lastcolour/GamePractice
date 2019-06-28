#include "Application.hpp"
#include "Platform.hpp"
#include "CoreModule.hpp"
#include "Render/RenderModule.hpp"
#include "Game/GameModule.hpp"
#include "Core/ETSystem.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/GameInitModule.hpp"

namespace {
    const int APP_SUCCESSED = 0;
    const int APP_FAILED = 1;
} // namespace

Application::Application(Application::PlatformPtrT&& runPlatform) :
    etSystem(new ETSystem()),
    platform(std::move(runPlatform)) {
}

Application::~Application() {
    deinit();
}

void Application::buildModules(ModuleListT& modules) {
    modules.emplace_back(new CoreModule);
    if (platform) {
        modules.emplace_back(platform->createPlatformModule());
    }
    modules.emplace_back(new RenderModule);
    modules.emplace_back(new GameModule);
    modules.emplace_back(new GameInitModule);
}

bool Application::init() {
    buildModules(systemModules);
    for(const auto& module : systemModules) {
        if(!module || !module->init()) {
            return false;
        }
    }
    return true;
}

int Application::run() {
    int retCode = APP_FAILED;
    if (init()) {
        mainLoop();
        retCode = APP_SUCCESSED;
    }
    deinit();
    return retCode;
}

void Application::deinit() {
    for(auto it = systemModules.rbegin(), end = systemModules.rend(); it != end; ++it) {
        it->reset();
    }
    systemModules.clear();
}

void Application::mainLoop() {
    bool needRun = true;
    while(needRun) {
        ET_SendEvent(&ETTimer::ET_onFrameStart);

        needRun = false;
        ET_SendEventReturn(needRun, &ETAppRunStateEvents::ET_isNeedRun);
    }
}