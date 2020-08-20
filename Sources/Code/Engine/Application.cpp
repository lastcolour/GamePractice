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
    std::unique_ptr<RunTask> inputUpdate(new RunTask("Input", [](){
        ET_SendEvent(&ETInputUpdateTask::ET_updateInput);
    }));
    inputUpdate->setType(RunTaskType::MainThreadOnly);
    inputUpdate->setFrequency(120);

    std::unique_ptr<RunTask> assetsUpdate(new RunTask("Assets", [](){
        ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets);
    }));
    assetsUpdate->setType(RunTaskType::NoInMainThread);
    assetsUpdate->setFrequency(120);

    std::unique_ptr<RunTask> entitiesUpdate(new RunTask("Entities", [](){
        ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
    }));
    entitiesUpdate->setFrequency(120);

    std::unique_ptr<RunTask> uiUpdate(new RunTask("UI", [](){
        ET_SendEvent(&ETUIUpdateTask::ET_updateUI);
    }));
    uiUpdate->setFrequency(60);

    std::unique_ptr<RunTask> gameUpdate(new RunTask("Game", [](){
        ET_SendEvent(&ETGameUpdateTask::ET_updateGame);
    }));
    gameUpdate->setFrequency(60);

    gameUpdate->addChild(uiUpdate);
    std::unique_ptr<RunTask> soundUpdate(new RunTask("Sound", [](){
        ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
    }));
    gameUpdate->setFrequency(60);

    std::unique_ptr<RunTask> renderUpdate(new RunTask("Render", [](){
        ET_SendEvent(&ETRenderUpdateTask::ET_updateRender);
    }));
    renderUpdate->setType(RunTaskType::MainThreadOnly);
    renderUpdate->setFrequency(120);

    TasksRunner runner;
    runner.addTask(std::move(inputUpdate));
    runner.addTask(std::move(soundUpdate));
    runner.addTask(std::move(assetsUpdate));
    runner.addTask(std::move(entitiesUpdate));
    runner.addTask(std::move(uiUpdate));
    runner.addTask(std::move(gameUpdate));
    runner.addTask(std::move(renderUpdate));
    runner.runUntil(4, [](){
        bool needRun = false;
        ET_SendEventReturn(needRun, &ETAppRunStateEvents::ET_isNeedRun);
        return needRun;
    });
}