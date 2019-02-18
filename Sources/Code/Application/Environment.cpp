#include "Environment.hpp"

#include <cassert>
#include <iostream>

Environment* Environment::ENV = nullptr;

Environment::Environment(Application& application) :
    app(application) {
}

Environment::~Environment() {
}

bool Environment::InitEnv(Application& application) {
    if(!ENV) {
        ENV = new Environment(application);
    } else {
        assert(false && "Environment already inited");
        return false;
    }
    return true;
}

void Environment::DeinitEnv() {
    delete ENV;
    ENV = nullptr;
}

Environment* GetEnv() {
    return Environment::ENV;
}

Application& Environment::getApp() {
    return app;
}

Platform& Environment::getPlatform() {
    return *app.platform;
}

Logger* Environment::getLogger() {
    return app.logger.get();
}

Assets* Environment::getAssets() {
    return app.assets.get();
}

Surface* Environment::getSurface() {
    return app.surface.get();
}

Render* Environment::getRender() {
    return app.render.get();
}

Game* Environment::getGame() {
    return app.game.get();
}