#ifndef __ANDROID_AUDIO_DEVICE_HPP__
#define __ANDROID_AUDIO_DEVICE_HPP__

#include "Core/SystemLogic.hpp"
#include "Platform/ETDevice.hpp"

class AndroidAudioDevice : public SystemLogic,
    public ETNode<ETDevice> {
public:

    AndroidAudioDevice();
    virtual ~AndroidAudioDevice();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETDevice
    const DeviceAudioConfig* ET_getAudioConfig() const override;

private:

    bool readDeviceConfig();

private:

    DeviceAudioConfig deviceConfig;
};

#endif /* __ANDROID_AUDIO_DEVICE_HPP__ */