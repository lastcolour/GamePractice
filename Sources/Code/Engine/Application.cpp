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
#include "UI/ETUITimer.hpp"
#include "Game/ETGameTimer.hpp"

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
    TasksRunner runner;
    {
        auto inputUpdate = runner.createTask("Input", []{
            ET_SendEvent(&ETInputUpdateTask::ET_updateInput);
        });
        inputUpdate->setType(RunTaskType::MainThreadOnly);
        inputUpdate->setFrequency(120);
    }
    {
        auto assetsUpdate = runner.createTask("Assets", [](){
            ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets);
        });
        assetsUpdate->setType(RunTaskType::NoInMainThread);
        assetsUpdate->setFrequency(120);
    }
    {
        auto entitiesUpdate = runner.createTask("Entities", [](){
            ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
        });
        entitiesUpdate->setFrequency(120);
    }
    {
        auto uiUpdate = runner.createTask("UI", [](){
            ET_SendEvent(&ETUITimer::ET_onTick);
        });
        auto renderSync = runner.createTask("RenderSync", [](){
            ET_SendEvent(&ETRenderUpdateTask::ET_syncWithGame);
        });
        renderSync->setType(RunTaskType::MainThreadOnly);
    
        auto gameUpdate = runner.createTask("Game", [](){
            ET_SendEvent(&ETGameTimer::ET_onTick);
        });

        gameUpdate->setFrequency(120);
        gameUpdate->addChild(uiUpdate);
        uiUpdate->addChild(renderSync);
    }
    {
        auto soundUpdate = runner.createTask("Sound", [](){
            ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
        });
        soundUpdate->setFrequency(60);
    }
    {
        auto renderUpdate = runner.createTask("Render", [](){
            ET_SendEvent(&ETRenderUpdateTask::ET_updateRender);
        });
        renderUpdate->setType(RunTaskType::MainThreadOnly);
        renderUpdate->setFrequency(120);
    }
    runner.runUntil(4, [](){
        bool needRun = false;
        ET_SendEventReturn(needRun, &ETAppRunStateEvents::ET_isNeedRun);
        return needRun;
    });
}