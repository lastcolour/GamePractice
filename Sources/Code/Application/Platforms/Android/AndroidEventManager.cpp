#include "Platforms/Android/AndroidEventManager.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

#include <cassert>

AndroidEventManager::AndroidEventManager() {
}

AndroidEventManager::~AndroidEventManager() {
}

bool AndroidEventManager::init() {
    if(!GetAndroindPlatformHandler()) {
        assert(false && "Invalid platform handler");
        return false;
    }
    ETNode<ETAndroidActivityEvents>::connect(getEntityId());
    ETNode<ETTimerEvents>::connect(getEntityId());
    return true;
}

void AndroidEventManager::deinit() {
    ETNode<ETAndroidActivityEvents>::disconnect();
    ETNode<ETTimerEvents>::disconnect();
}

void AndroidEventManager::ET_onTick(float dt) {
    GetAndroindPlatformHandler()->pollEvents();
}

void AndroidEventManager::ET_onActivityEvent(ActivityEventType eventType) {
    switch(eventType) {
        case ActivityEventType::OnStart:
            ET_SendEvent(&ETAppRunStateEvents::ET_onResume);
            break;
        case ActivityEventType::OnResume:
            ET_SendEvent(&ETAppRunStateEvents::ET_onResume);
            break;
        case ActivityEventType::OnPause:
            ET_SendEvent(&ETAppRunStateEvents::ET_onPause);
            break;
        case ActivityEventType::OnStop:
            ET_SendEvent(&ETAppRunStateEvents::ET_onPause);
            break;
        case ActivityEventType::OnDestroy:
            ET_SendEvent(&ETAppRunStateEvents::ET_onTerminate);
            break;
        default:
            break;
    }
}