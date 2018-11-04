#include "Application.hpp"
#include "Platform.hpp"

Application::Application(Platform* pltfrm) :
    platform(pltfrm) {
}

Application::~Application() {
}

int Application::run() {
    if(!platform || !platform->init()) {
        return -1;
    }
    mainLoop();
    return 0;
}

void Application::mainLoop() {
}