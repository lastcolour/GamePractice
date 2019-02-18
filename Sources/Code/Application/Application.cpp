#include "Application.hpp"
#include "Platform.hpp"
#include "Environment.hpp"
#include "ModuleFactory.hpp"
#include "Logger.hpp"
#include "Assets.hpp"
#include "Surface.hpp"
#include "Render/Render.hpp"
#include "Game/Game.hpp"

Application::Application(Platform* pltfrm) :
    platform(pltfrm),
    logger(),
    assets(),
    surface(),
    render(),
    game() {

    if(platform && platform->init()) {
        Environment::InitEnv(*this);
    } else {
        platform.reset();
    }
}

Application::~Application() {
    Environment::DeinitEnv();
}

std::unique_ptr<ModuleFactory> Application::createModuleFactory() {
    return std::unique_ptr<ModuleFactory>(new ModuleFactory);
}

bool Application::initLogger(Logger& logger) {
    logger.setLogLevel(LogLevel::Debug);
    return true;
}

bool Application::initAssets(Assets& assets) {
    return assets.init();
}

bool Application::initSurface(Surface& surface) {
    if(!surface.init()) {
        return false;
    }
    if(!surface.show()) {
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
    if(!platform) {
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