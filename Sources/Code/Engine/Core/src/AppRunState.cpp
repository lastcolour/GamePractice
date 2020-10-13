#include "Core/App/AppRunState.hpp"

AppRunState::AppRunState() :
    needRun(true) {
}

bool AppRunState::init() {
    ETNode<ETAppRunStateEvents>::connect(getEntityId());
    return true;
}

void AppRunState::deinit() {
    ETNode<ETAppRunStateEvents>::disconnect();
}

void AppRunState::ET_onTerminate() {
    needRun = false;
}

bool AppRunState::ET_isNeedRun() const {
    return needRun;
}

void AppRunState::ET_onPause() {
}

void AppRunState::ET_onResume() {
}