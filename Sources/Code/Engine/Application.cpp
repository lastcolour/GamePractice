#include "Application.hpp"
#include "Core/ETApplication.hpp"
#include "CoreModule.hpp"
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

#include <cassert>

namespace {

const int APP_THREAD_COUNT = 4;
const int MAX_GAME_TICK_RATE = 60;
const int AUX_TASK_TICK_RATE = MAX_GAME_TICK_RATE * 2;
const int MEMORY_TICK_RATE = 30;

} // namespace

Application::Application() :
    globalEnv(new Core::GlobalEnvironment()) {
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
    auto runner = GetEnv()->GetTasksRunner();
    assert(runner && "Invalid task runner");
    {
        auto memoryUpdate = runner->createTask("Memory", [](float dt){
            GetEnv()->GetMemoryAllocator()->update(dt);
        });
        memoryUpdate->setFrequency(MEMORY_TICK_RATE);
    }
    {
        auto inputUpdate = runner->createTask("Input", [](){
            ET_SendEvent(&ETInputUpdateTask::ET_updateInput);
        });
        inputUpdate->setType(RunTaskType::MainThreadOnly);
        inputUpdate->setFrequency(AUX_TASK_TICK_RATE);
    }
    {
        auto assetsUpdate = runner->createTask("Assets", [](float dt){
            ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets, dt);
        });
        assetsUpdate->setType(RunTaskType::NoInMainThread);
        assetsUpdate->setFrequency(AUX_TASK_TICK_RATE);
    }
    {
        auto entitiesUpdate = runner->createTask("Entities", [](){
            ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
        });
        entitiesUpdate->setType(RunTaskType::NoInMainThread);
        entitiesUpdate->setFrequency(AUX_TASK_TICK_RATE);
    }
    {
        auto soundUpdate = runner->createTask("Sound", [](){
            ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
        });
        soundUpdate->setFrequency(AUX_TASK_TICK_RATE);
    }

    auto uiUpdate = runner->createTask("UI", [](float dt){
        ET_SendEvent(&ETUITimer::ET_onTick, dt);
    });
    uiUpdate->setFrequency(MAX_GAME_TICK_RATE);

    auto gameUpdate = runner->createTask("Game", [](float dt){
        ET_SendEvent(&ETGameTimer::ET_onTick, dt);
    });
    gameUpdate->setFrequency(MAX_GAME_TICK_RATE);

    auto renderSync = runner->createTask("RenderSync", [](){
        ET_SendEvent(&ETRenderUpdateTask::ET_syncWithGame);
    });
    renderSync->setFrequency(MAX_GAME_TICK_RATE);
    renderSync->setType(RunTaskType::MainThreadOnly);

    auto renderUpdate = runner->createTask("Render", [](){
        ET_SendEvent(&ETRenderUpdateTask::ET_updateRender);
    });
    renderUpdate->setType(RunTaskType::MainThreadOnly);
    renderUpdate->setFrequency(MAX_GAME_TICK_RATE);

    auto particlesUpdate = runner->createTask("Particles", [](float dt){
        ET_SendEvent(&ETRenderUpdateTask::ET_updateParticles, dt);
    });
    particlesUpdate->setFrequency(MAX_GAME_TICK_RATE);

    gameUpdate->addChild(uiUpdate);
    gameUpdate->addChild(renderSync);
    gameUpdate->addChild(particlesUpdate);

    uiUpdate->addChild(renderSync);
    uiUpdate->addChild(particlesUpdate);

    renderSync->addChild(renderUpdate);
    particlesUpdate->addChild(renderUpdate);

    runner->start(APP_THREAD_COUNT);
}