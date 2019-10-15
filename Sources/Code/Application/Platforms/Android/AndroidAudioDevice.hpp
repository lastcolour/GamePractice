#ifndef __ANDROID_AUDIO_DEVICE_HPP__
#define __ANDROID_AUDIO_DEVICE_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platforms/Android/ETAndroidInterfaces.hpp"

class AndroidAudioDevice : public SystemLogic,
    public ETNode<ETAndroidAudioDevice> {
public:

    AndroidAudioDevice();
    virtual ~AndroidAudioDevice();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAndroidAudioDevice
    const AndroidAudioDeviceConfig* ET_getAudioConfig() const override;

private:

    AndroidAudioDeviceConfig config;
};

#endif /* __ANDROID_AUDIO_DEVICE_HPP__ */