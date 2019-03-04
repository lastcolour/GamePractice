#include "ModuleFactory.hpp"
#include "Platform.hpp"
#include "Game/Game.hpp"
#include "Render/Render.hpp"

std::unique_ptr<Logger> ModuleFactory::createLogger() {
    return GetEnv()->getPlatform()->createLogger();
}

std::unique_ptr<Assets> ModuleFactory::createAssets() {
    return GetEnv()->getPlatform()->createAssets();
}

std::unique_ptr<Surface> ModuleFactory::createSurface() {
    return GetEnv()->getPlatform()->createSurface();
}

std::unique_ptr<Render> ModuleFactory::createRender() {
    return std::unique_ptr<Render>(new Render);
}

std::unique_ptr<Game> ModuleFactory::createGame() {
    return std::unique_ptr<Game>(new Game);
}