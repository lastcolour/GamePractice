#include "Application.hpp"
#include "Platform.hpp"
#include "CoreModule.hpp"
#include "Render/RenderModule.hpp"
#include "Game/GameModule.hpp"
#include "Core/ETSystem.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {
    const int APP_SUCCESSED = 0;
    const int APP_FAILED = -1;
} // namespace

Application::Application(Application::PlatformPtrT&& runPlatform) :
    etSystem(new ETSystem()),
    platform(std::move(runPlatform)) {
}

Application::~Application() {
}

Application::ModuleListT&& Application::createModules() {
    ModuleListT moduleList;
    moduleList.emplace_back(new CoreModule);
    moduleList.emplace_back(platform ? platform->createPlatformModule() : nullptr);
    moduleList.emplace_back(new RenderModule);
    moduleList.emplace_back(new GameModule);
    return std::move(moduleList);
}

bool Application::init() {
    systemModules = createModules();
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
    deinit();
    return APP_SUCCESSED;
}

void Application::deinit() {
    for(auto it = systemModules.rbegin(), end = systemModules.rend(); it != end; ++it) {
        it->reset();
    }
}

void Application::mainLoop() {
    bool needRun = true;
    while(needRun) {
        ET_SendEvent(&ETTimer::ET_onFrameStart);

        needRun = false;
        ET_SendEventReturn(needRun, &ETAppRunStateEvents::ET_isNeedRun);
    }
}