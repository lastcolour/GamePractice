#include "Application.hpp"
#include "Platform.hpp"
#include "Environment.hpp"
#include "Game/Game.hpp"

Application::Application(Platform* pltfrm) :
    platform(pltfrm),
    game(nullptr) {

    Environment::InitEnv(*this);
}

Application::~Application() {
    Environment::DeinitEnv();
}

std::unique_ptr<Game> Application::createGame() {
    return std::unique_ptr<Game>(new Game);
}

int Application::run() {
    if(!platform || !platform->init()) {
        return -1;
    }
    game = std::move(createGame());
    if(!game || !game->init()) {
        return -1;
    }
    mainLoop();
    return 0;
}

void Application::mainLoop() {
    while(platform->shouldRun() && game->shouldRun()) {
        platform->update();
        game->update();
    }
}