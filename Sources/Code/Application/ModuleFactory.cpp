#include "ModuleFactory.hpp"
#include "Platform.hpp"
#include "Logger.hpp"
#include "Assets.hpp"
#include "Surface.hpp"
#include "Game/Game.hpp"
#include "Render/Render.hpp"

std::unique_ptr<Logger> ModuleFactory::createLogger() {
    auto logStream = GetEnv()->getPlatform().createLogStream();
    if(!logStream) {
        return nullptr;
    }
    return std::unique_ptr<Logger>(new Logger(std::move(logStream)));
}

std::unique_ptr<Assets> ModuleFactory::createAssets() {
    return GetEnv()->getPlatform().createAssets();
}

std::unique_ptr<Surface> ModuleFactory::createSurface() {
    auto& platform = GetEnv()->getPlatform();
    return platform.createSurface();
}

std::unique_ptr<Render> ModuleFactory::createRender() {
    return std::unique_ptr<Render>(new Render);
}

std::unique_ptr<Game> ModuleFactory::createGame() {
    return std::unique_ptr<Game>(new Game);
}