#include "Application.hpp"
#include "Platform.hpp"
#include "Environment.hpp"
#include "ModuleFactory.hpp"
#include "Core/ETSystem.hpp"
#include "Render/Render.hpp"
#include "Game/Game.hpp"

Application::Application(Platform* pltfrm) :
    etSystem(new ETSystem),
    platform(pltfrm),
    logger(),
    assets(),
    surface(),
    render(),
    game() {

    Environment::InitEnv(*this);
}

Application::~Application() {
    game.reset();
    render.reset();
    surface.reset();
    assets.reset();
    logger.reset();
    platform.reset();
    etSystem.reset();
    Environment::DeinitEnv();
}

std::unique_ptr<ModuleFactory> Application::createModuleFactory() {
    return std::unique_ptr<ModuleFactory>(new ModuleFactory);
}

bool Application::initLogger(Logger& logger) {
    if(!logger.init()) {
        return false;
    }
    logger.ET_setLogLevel(LogLevel::Debug);
    return true;
}

bool Application::initAssets(Assets& assets) {
    return assets.init();
}

bool Application::initSurface(Surface& surface) {
    if(!surface.init()) {
        return false;
    }
    if(!surface.ET_show()) {
        return false;
    }
    return true;
}

bool Application::initRender(Render& render) {
    return render.init();
}

bool Application::initGame(Game& game) {
    return game.init();
}

bool Application::init() {
    if(!platform || !platform->init()) {
        return false;
    }
    auto moduleFactory = createModuleFactory();
    if(!moduleFactory) {
        return false;
    }
    logger = moduleFactory->createLogger();
    if(!logger || !initLogger(*logger)) {
        return false;
    }
    assets = moduleFactory->createAssets();
    if(!assets || !initAssets(*assets)) {
        LogError("[Application] Can't init with invalid assets");
        return false;
    }
    surface = moduleFactory->createSurface();
    if(!surface || !initSurface(*surface)) {
        LogError("[Application] Can't init with invalid surface");
        return false;
    }
    render = moduleFactory->createRender();
    if(!render || !initRender(*render)) {
        LogError("[Application] Can't init with invalid render");
        return false;
    }
    game = moduleFactory->createGame();
    if(!game || !initGame(*game)) {
        LogError("[Application] Can't init with invalid game");
        return false;
    }
    return true;
}

int Application::run() {
    if(!init()) {
        return -1;
    }
    mainLoop();
    return 0;
}

void Application::mainLoop() {
    while(true) {
        surface->update();
        if(!surface->shouldRun()) {
            LogDebug("[Application] Break mainLoop due to Surface");
            break;
        }
        game->update();
        if(!game->shouldRun()) {
            LogDebug("[Application] Break mainLoop due to Game");
            break;
        }
        render->update();
    }
}