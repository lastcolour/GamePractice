#include "Application.hpp"
#include "Core/ETApplication.hpp"
#include "Core/CoreModule.hpp"
#include "Render/RenderModule.hpp"
#include "Audio/AudioModule.hpp"
#include "Entity/EntityModule.hpp"
#include "UI/UIModule.hpp"
#include "Laucher/GameLaucherModule.hpp"
#include "Game/GameModule.hpp"
#include "Platform/PlatformModule.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "Core/ETTasks.hpp"

Application::Application() :
    etSystem(new ETSystem()) {
}

Application::~Application() {
    deinit();
}

void Application::buildModules(ModuleListT& modules) {
    modules.emplace_back(new PlatformModule);
    modules.emplace_back(new EntityModule);
    modules.emplace_back(new RenderModule);
    modules.emplace_back(new AudioModule);
    modules.emplace_back(new UIModule);
    modules.emplace_back(new GameModule);
    modules.emplace_back(new GameLaucherModule);
}

bool Application::init() {
    systemModules.emplace_back(new CoreModule);
    buildModules(systemModules);
    for(const auto& module : systemModules) {
        if(!module || !module->init()) {
            deinit();
            return false;
        }
    }
    return true;
}

int Application::run() {
    int retCode = 1;
    if(init()) {
        mainLoop();
        retCode = 0;
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
    RunTask inputUpdate([](){
        ET_SendEvent(&ETInputUpdateTask::ET_updateInput);
    });
    inputUpdate.setType(RunTaskType::MainThreadOnly);
    RunTask assetsUpdate([](){
        ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets);
    });
    assetsUpdate.setType(RunTaskType::NoInMainThread);
    RunTask entitiesUpdate([](){
        ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
    });
    RunTask uiUpdate([](){
        ET_SendEvent(&ETUIUpdateTask::ET_updateUI);
    });
    RunTask soundUpdate([](){
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
    });
    RunTask gameUpdate([](){
        ET_SendEvent(&ETGameUpdateTask::ET_updateGame);
    });
    RunTask renderUpdate([](){
        ET_SendEvent(&ETRenderUpdateTask::ET_updateRender);
    });
    renderUpdate.setType(RunTaskType::MainThreadOnly);

    TasksRunner runner;
    runner.addTask(inputUpdate);
    runner.addTask(soundUpdate);
    runner.addTask(assetsUpdate);
    runner.addTask(entitiesUpdate);
    runner.addTask(uiUpdate);
    runner.addTask(gameUpdate);
    runner.addTask(renderUpdate);
    runner.runUntil(2, [](){
        bool needRun = false;
        ET_SendEventReturn(needRun, &ETAppRunStateEvents::ET_isNeedRun);
        return needRun;
    });
}