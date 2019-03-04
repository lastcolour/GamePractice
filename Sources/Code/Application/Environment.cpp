#include "Environment.hpp"
#include "Application.hpp"

#include <cassert>
#include <iostream>

Environment* Environment::ENV = nullptr;

Environment::Environment(Application& application) :
    app(&application) {
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

ETSystem* Environment::getETSystem() {
    if(app) {
        return app->etSystem.get();
    } else {
        return nullptr;
    }
}

Platform* Environment::getPlatform() {
    if(app) {
        return app->platform.get();
    } else {
        return nullptr;
    }
}