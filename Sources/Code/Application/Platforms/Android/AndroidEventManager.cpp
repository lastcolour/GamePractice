#include "Platforms/Android/AndroidEventManager.hpp"
#include "Platforms/Android/AndroidPlatformHandler.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>

#include <android/input.h>

namespace {

void printActivityEvent(ActivityEventType eventType) {
    const char* eventName = "Unknown";
    switch(eventType) {
        case ActivityEventType::OnStart: {
            eventName = "OnStart";
            break;
        }
        case ActivityEventType::OnPause: {
            eventName = "OnPause";
            break;
        }
        case ActivityEventType::OnStop: {
            eventName = "OnStop";
            break;
        }
        case ActivityEventType::OnDestroy: {
            eventName = "OnDestroy";
            break;
        }
        case ActivityEventType::OnResume: {
            eventName = "OnResume";
            break;
        }
        case ActivityEventType::OnLowMemory: {
            eventName = "OnLowMemory";
            break;
        }
        case ActivityEventType::OnConfigurationChanged: {
            eventName = "OnConfigurationChanged";
            break;
        }
        case ActivityEventType::OnWindowFocusGet: {
            eventName = "OnWindowFocusGet";
            break;
        }
        case ActivityEventType::OnWindowFocusLost: {
            eventName = "OnWindowFocusLost";
            break;
        }
        case ActivityEventType::OnNativeWindowChanged: {
            eventName = "OnNativeWindowChanged";
            break;
        }
        case ActivityEventType::OnInputQueueChanged: {
            eventName = "OnInputQueueChanged";
            break;
        }
        default:
            break;
    }
    LogInfo("[printActivityEvent] OnActivityEvent: %s", eventName);
}

} // namespace

AndroidEventManager::AndroidEventManager() {
}

AndroidEventManager::~AndroidEventManager() {
}

bool AndroidEventManager::init() {
    if(!GetAndroindPlatformHandler()) {
        assert(false && "Invalid platform handler");
        return false;
    }
    ETNode<ETAndroidInputEvents>::connect(getEntityId());
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

bool AndroidEventManager::hadleMotionEvent(AInputEvent* inputEvent) {
    int32_t eventSource = AInputEvent_getSource(inputEvent);
    if(eventSource != AINPUT_SOURCE_TOUCHSCREEN) {
        return false;
    }

    Vec2i touchPt(0);
    touchPt.x = static_cast<int>(AMotionEvent_getX(inputEvent, 0));
    touchPt.y = static_cast<int>(AMotionEvent_getY(inputEvent, 0));

    Vec2i size(0);
    ET_SendEventReturn(size, &ETSurface::ET_getSize);
    touchPt.y = size.y - touchPt.y;

    int32_t eventAction = AKeyEvent_getAction(inputEvent);
    switch (eventAction)
    {
    case AMOTION_EVENT_ACTION_DOWN: {
        ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Press, touchPt);
        return true;
    }
    case AMOTION_EVENT_ACTION_MOVE: {
        ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Move, touchPt);
        return true;
    }
    case AMOTION_EVENT_ACTION_UP: {
        ET_SendEvent(&ETInputEvents::ET_onTouch, EActionType::Release, touchPt);
        return true;
    }
    default:
        return false;
    }
}

bool AndroidEventManager::handleKeyEvent(AInputEvent* inputEvent) {
    int32_t keyCode = AKeyEvent_getKeyCode(inputEvent);
    if(keyCode != AKEYCODE_BACK) {
        return false;
    }
    int32_t eventAction = AKeyEvent_getAction(inputEvent);
    if (eventAction == AKEY_EVENT_ACTION_DOWN) {
        ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Press, EButtonId::Back);
        return true;
    } else if(eventAction == AKEY_EVENT_ACTION_UP) {
        ET_SendEvent(&ETInputEvents::ET_onButton, EActionType::Release, EButtonId::Back);
        return true;
    }
    return false;
}

bool AndroidEventManager::ET_onInputEvent(AInputEvent* inputEvent) {
    switch (AInputEvent_getType(inputEvent)) {
        case AINPUT_EVENT_TYPE_MOTION: {
            return hadleMotionEvent(inputEvent);
        }
        case AINPUT_EVENT_TYPE_KEY: {
            return handleKeyEvent(inputEvent);
        }
        default: {
            return false;
        }
    }
}

void AndroidEventManager::ET_onActivityEvent(ActivityEventType eventType) {
    printActivityEvent(eventType);
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