#ifndef __ANDROID_PLATFORM_HANDLER_HPP__
#define __ANDROID_PLATFORM_HANDLER_HPP__

#include "Platforms/Android/ETAndroidInterfaces.hpp"
#include "Platforms/Android/JNIHelpers.hpp"

#include <memory>
#include <mutex>
#include <condition_variable>
#include <vector>

class Application;

struct ANativeActivity;
struct ANativeWindow;
struct AInputQueue;
struct AConfiguration;
struct ALooper;
struct AAssetManager;

enum class LooperQueueId {
    ActivityEvents = 1,
    Input = 2
};

class AndroindPlatformHandler {
public:

    AndroindPlatformHandler(ANativeActivity* activity, void* savedState, size_t savedStateSize);
    ~AndroindPlatformHandler();

    void start();
    void pollEvents();
    AAssetManager* getAssetsManager();
    ANativeWindow* getWindow();
    const char* getInternalPath() const;

    JNI::JVObject getActivityJavaObject();
    void attachToJavaVMThread(JNIEnv*& jv_env);
    void detachFromJavaVMTrehad();

private:

    static void onDestroy(ANativeActivity* activity);
    static void onStart(ANativeActivity* activity);
    static void onResume(ANativeActivity* activity);
    static void onPause(ANativeActivity* activity);
    static void onStop(ANativeActivity* activity);
    static void onLowMemory(ANativeActivity* activity);
    static void onConfigurationChanged(ANativeActivity* activity);
    static void onWindowFocusChanged(ANativeActivity* activity, int focused);
    static void onNativeWindowCreated(ANativeActivity* activity, ANativeWindow* window);
    static void onNativeWindowDestroyed(ANativeActivity* activity, ANativeWindow* window);
    static void onInputQueueCreated(ANativeActivity* activity, AInputQueue* queue);
    static void onInputQueueDestroyed(ANativeActivity* activity, AInputQueue* queue);
    static void* onSaveInstanceState(ANativeActivity* activity, size_t* outLen);

private:

    bool initHandler();
    void deinitHandler(); 

    void initAppThread();
    void deinitAppThread();
    void onActivityEvent(ActivityEventType eventType);

    void pollInputEvents();
    void pollActivityEvents();

    void markEventHandled(ActivityEventType eventType);
    void waitUntilEventHandler(ActivityEventType eventType);

private:

    AInputQueue* pendingInputQueue;
    ANativeWindow* nativeWindow;

    ANativeActivity* nativeActivity;
    AInputQueue* inputQueue;
    AConfiguration* config;
    ALooper* looper;

    int eventsWritePipe;
    int eventsReadPipe;

    bool isRunning;
    bool isDestroyed;
    std::mutex mutex;
    std::condition_variable cond;
    std::unique_ptr<Application> app;

    std::vector<int> handledEventMap;
};

AndroindPlatformHandler* GetAndroindPlatformHandler();

#endif /* __ANDROID_PLATFORM_HANDLER_HPP__ */