#include "Environment.hpp"

#include <cassert>

Environment* Environment::ENV = nullptr;

Environment::Environment(Application& application) :
    app(application),
    surface(nullptr) {
}

Environment::~Environment() {
}

void Environment::InitEnv(Application& application) {
    if(!ENV)
        ENV = new Environment(application);
    else
        assert(false && "Environment already inited");
}

void Environment::DeinitEnv() {
    delete ENV;
    ENV = nullptr;
}

Application& Environment::getApp() {
    return app;
}

void Environment::registerSurface(Surface* surf) {
    surface = surf;
}

Surface* Environment::getSurface() {
    return surface;
}

Environment* GetEnv() {
    return Environment::ENV;
}