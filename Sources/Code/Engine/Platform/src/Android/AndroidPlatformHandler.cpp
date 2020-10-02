#include "Android/AndroidPlatformHandler.hpp"
#include "Application.hpp"
#include "Core/ETPrimitives.hpp"

#include <thread>
#include <cerrno>
#include <unistd.h>

#include <android/looper.h>
#include <android/configuration.h>
#include <android/native_activity.h>
#include <android/log.h>

#define LogAndroidError(...) ((void)__android_log_print(ANDROID_LOG_ERROR, nullptr, __VA_ARGS__))

namespace {

AndroindPlatformHandler* HANDLER = nullptr;

const int NullPipe = -1;
const int32_t InputEventHandled = 1;
const int32_t InputEventNotHandled = 0;

} // namespace

AndroindPlatformHandler* GetAndroindPlatformHandler() {
    assert(HANDLER && "Invalid Handler");
    return HANDLER;
}

AndroindPlatformHandler::AndroindPlatformHandler(ANativeActivity* activity, void* savedState, size_t savedStateSize) :
    app(nullptr),
    nativeActivity(activity),
    inputQueue(nullptr),
    config(nullptr),
    nativeWindow(nullptr),
    looper(nullptr),
    eventsWritePipe(::NullPipe),
    eventsReadPipe(::NullPipe),
    isAppCreated(false),
    isAppTreadDestroyed(false),
    pendingInputQueue(nullptr),
    handledEventMap(static_cast<size_t>(ActivityEventType::EVENT_COUNT), 0),
    onDestroyPolled(false) {
}

AndroindPlatformHandler::~AndroindPlatformHandler() {
    assert(HANDLER == this && "Invalid handler");
    HANDLER = nullptr;
}

bool AndroindPlatformHandler::initHandler() {
    if(HANDLER) {
        LogAndroidError("[AndroindPlatformHandler::initHandler] Already exist one android handler");
        return false;
    }

    HANDLER = this;

    int tEventPipe[2];
    if(pipe(tEventPipe)) {
        LogAndroidError("[AndroindPlatformHandler::initHandler] Can't open event pipe: %s", strerror(errno));
        return false;
    }
    eventsReadPipe = tEventPipe[0];
    eventsWritePipe = tEventPipe[1];

    nativeActivity->callbacks->onDestroy = onDestroy;
    nativeActivity->callbacks->onStart = onStart;
    nativeActivity->callbacks->onResume = onResume;
    nativeActivity->callbacks->onSaveInstanceState = onSaveInstanceState;
    nativeActivity->callbacks->onPause = onPause;
    nativeActivity->callbacks->onStop = onStop;
    nativeActivity->callbacks->onConfigurationChanged = onConfigurationChanged;
    nativeActivity->callbacks->onLowMemory = onLowMemory;
    nativeActivity->callbacks->onWindowFocusChanged = onWindowFocusChanged;
    nativeActivity->callbacks->onNativeWindowCreated = onNativeWindowCreated;
    nativeActivity->callbacks->onNativeWindowDestroyed = onNativeWindowDestroyed;
    nativeActivity->callbacks->onInputQueueCreated = onInputQueueCreated;
    nativeActivity->callbacks->onInputQueueDestroyed = onInputQueueDestroyed;
    nativeActivity->instance = static_cast<void*>(this);
    return true;
}

void AndroindPlatformHandler::deinitHandler() {
    while(!isAppTreadDestroyed.load()) {
        std::this_thread::yield();
    }

    if(eventsReadPipe != ::NullPipe && eventsWritePipe != ::NullPipe) {
        close(eventsReadPipe);
        close(eventsWritePipe);
    }
}

void AndroindPlatformHandler::initAppThread() {
    config = AConfiguration_new();
    AConfiguration_fromAssetManager(config, nativeActivity->assetManager);
    looper = ALooper_prepare(ALOOPER_PREPARE_ALLOW_NON_CALLBACKS);
    ALooper_addFd(looper, eventsReadPipe, static_cast<int>(LooperQueueId::ActivityEvents),
        ALOOPER_EVENT_INPUT, nullptr, nullptr);
}

void AndroindPlatformHandler::deinitAppThread() {
    if(inputQueue) {
        AInputQueue_detachLooper(inputQueue);
    }
    AConfiguration_delete(config);

    isAppTreadDestroyed.store(true);
}

void AndroindPlatformHandler::waitOnDestroyEvent() {
    if(onDestroyPolled) {
        return;
    }
    ANativeActivity_finish(nativeActivity);
    while(!onDestroyPolled) {
        pollEvents();
    }
}

void AndroindPlatformHandler::start() {
    if(!initHandler()) {
        return;
    }

    std::thread thread([this](){
        initAppThread();
        {
            Application app;
            isAppCreated.store(true);
            app.run();
            waitOnDestroyEvent();
        }
        deinitAppThread();
    });
    thread.detach();

    while(!isAppCreated.load()) {
        std::this_thread::yield();
    }
}

AAssetManager* AndroindPlatformHandler::getAssetsManager() {
    assert(nativeActivity && "Invalid activity");
    return nativeActivity->assetManager;
}

ANativeWindow* AndroindPlatformHandler::getWindow() {
    return nativeWindow;
}

const char* AndroindPlatformHandler::getInternalPath() const {
    return nativeActivity->internalDataPath;
}

void* AndroindPlatformHandler::getActivityJavaObject() {
    return nativeActivity->clazz;
}

void* AndroindPlatformHandler::getJavaVM() {
    return nativeActivity->vm;
}

void AndroindPlatformHandler::pollEvents() {
    int tFdesc;
    int tEvents;
    int tIntQueueId;
    while((tIntQueueId = ALooper_pollAll(0, &tFdesc, &tEvents, nullptr)) >= 0) {
        LooperQueueId tQueuId = static_cast<LooperQueueId>(tIntQueueId);
        switch(tQueuId) {
            case LooperQueueId::Input: {
                pollInputEvents();
                break;
            }
            case LooperQueueId::ActivityEvents: {
                pollActivityEvents();
                break;
            }
            default: {
                LogAndroidError("[AndroindPlatformHandler::pollEvents] Invalid input queue id: %d", tQueuId);
                break;
            }
        }
    }
}

void AndroindPlatformHandler::pollInputEvents() {
    AInputEvent* inputEvent = nullptr;
    if(AInputQueue_getEvent(inputQueue, &inputEvent) < 0) {
        LogAndroidError("[AndroindPlatformHandler::pollInputEvents] Can't poll input event");
        return;
    }
    if(AInputQueue_preDispatchEvent(inputQueue, inputEvent)) {
        return;
    }
    bool isHandled = false;
    ET_SendEventReturn(isHandled, &ETAndroidInputEvents::ET_onInputEvent, inputEvent);
    AInputQueue_finishEvent(inputQueue, inputEvent,
        isHandled ? ::InputEventHandled : ::InputEventNotHandled);
}

void AndroindPlatformHandler::pollActivityEvents() {
    ActivityEventType eventType;
    if(read(eventsReadPipe, &eventType, sizeof(ActivityEventType)) != sizeof(ActivityEventType)) {
        LogAndroidError("[AndroindPlatformHandler::pollActivityEvents] Can't poll activity event from pipe: %s", strerror(errno));
        return;
    }

    if(eventType == ActivityEventType::OnInputQueueChanged) {
        if(inputQueue) {
            AInputQueue_detachLooper(inputQueue);
            inputQueue = nullptr;
        }
        if(pendingInputQueue) {
            AInputQueue_attachLooper(pendingInputQueue, looper, static_cast<int>(LooperQueueId::Input), nullptr, nullptr);
            inputQueue = pendingInputQueue;
        }
    } else if (eventType == ActivityEventType::OnDestroy) {
        onDestroyPolled = true;
    }
    ET_SendEvent(&ETAndroidActivityEvents::ET_onActivityEvent, eventType);
    markEventHandled(eventType);
}

void AndroindPlatformHandler::onActivityEvent(ActivityEventType eventType) {
    ET_SendEvent(&ETAndroidMainThreadActivityEvents::ET_onMainThreadActivityEvent, eventType);
    if(write(eventsWritePipe, &eventType, sizeof(ActivityEventType)) != sizeof(ActivityEventType)) {
        LogAndroidError("[AndroindPlatformHandler::onActivityEvent] Can't write android event to pipe: %s", strerror(errno));
        return;
    }
    waitUntilEventHandler(eventType);
}

void AndroindPlatformHandler::onDestroy(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnDestroy);
    handler->deinitHandler();

    delete handler;
    activity->instance = nullptr;
}

void AndroindPlatformHandler::onPause(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnPause);
}

void AndroindPlatformHandler::onStart(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnStart);
}

void AndroindPlatformHandler::onStop(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnStop);
}

void AndroindPlatformHandler::onResume(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnResume);
}

void AndroindPlatformHandler::onLowMemory(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnLowMemory);
}

void AndroindPlatformHandler::onConfigurationChanged(ANativeActivity* activity) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(ActivityEventType::OnConfigurationChanged);
}

void AndroindPlatformHandler::onWindowFocusChanged(ANativeActivity* activity, int focused) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->onActivityEvent(focused ? ActivityEventType::OnWindowFocusGet : ActivityEventType::OnWindowFocusLost);
}

void AndroindPlatformHandler::onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->nativeWindow = window;
    handler->onActivityEvent(ActivityEventType::OnNativeWindowChanged);
}

void AndroindPlatformHandler::onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->nativeWindow = nullptr;
    handler->onActivityEvent(ActivityEventType::OnNativeWindowChanged);
}

void AndroindPlatformHandler::onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->pendingInputQueue = queue;
    handler->onActivityEvent(ActivityEventType::OnInputQueueChanged);
}

void AndroindPlatformHandler::onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue) {
    auto handler = static_cast<AndroindPlatformHandler*>(activity->instance);
    handler->pendingInputQueue = nullptr;
    handler->onActivityEvent(ActivityEventType::OnInputQueueChanged);
}

void* AndroindPlatformHandler::onSaveInstanceState(ANativeActivity* activity, size_t* outLen) {
    *outLen = 0;
    return nullptr;
}

void AndroindPlatformHandler::markEventHandled(ActivityEventType eventType) {
    {
        std::lock_guard<std::mutex> lock(mutex);
        handledEventMap[static_cast<size_t>(eventType)] = true;
    }
    cond.notify_one();
}

void AndroindPlatformHandler::waitUntilEventHandler(ActivityEventType eventType) {
    std::unique_lock<std::mutex> uniqueLock(mutex);
    cond.wait(uniqueLock, [this, eventType](){
        if(handledEventMap[static_cast<size_t>(eventType)]) {
            handledEventMap[static_cast<size_t>(eventType)] = false;
            return true;
        }
        return false;
    });
}