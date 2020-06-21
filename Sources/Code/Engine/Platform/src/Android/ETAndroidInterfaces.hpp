#ifndef __ET_ANDROIND_INTERFACES_HPP__
#define __ET_ANDROIND_INTERFACES_HPP__

struct AInputEvent;

enum class ActivityEventType {
    OnStart = 0,
    OnPause,
    OnStop,
    OnDestroy,
    OnResume,
    OnLowMemory,
    OnConfigurationChanged,
    OnWindowFocusGet,
    OnWindowFocusLost,
    OnNativeWindowChanged,
    OnInputQueueChanged,
    EVENT_COUNT = OnInputQueueChanged,
};

struct ETAndroidActivityEvents {
    virtual ~ETAndroidActivityEvents() = default;
    virtual void ET_onActivityEvent(ActivityEventType eventType) = 0;
};

struct ETAndroidMainThreadActivityEvents {
    virtual ~ETAndroidMainThreadActivityEvents() = default;
    virtual void ET_onMainThreadActivityEvent(ActivityEventType eventType) = 0;
};

struct ETAndroidInputEvents {
    virtual ~ETAndroidInputEvents() = default;
    virtual bool ET_onInputEvent(AInputEvent* inputEvent) = 0;
};

struct AndroidAudioDeviceConfig {
    int frameRate;
    int framesPerBurst;

    AndroidAudioDeviceConfig() : frameRate(48000), framesPerBurst(960) {}
};

struct ETAndroidAudioDevice {
    virtual ~ETAndroidAudioDevice() = default;
    virtual const AndroidAudioDeviceConfig* ET_getAudioConfig() const = 0;
};

#endif /* __ET_ANDROIND_INTERFACES_HPP__ */