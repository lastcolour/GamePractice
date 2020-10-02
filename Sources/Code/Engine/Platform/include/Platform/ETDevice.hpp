#ifndef __ET_DEVICE_HPP__
#define __ET_DEVICE_HPP__

struct DeviceAudioConfig {
public:

    int frameRate;
    int framesPerBurst;

public:

    DeviceAudioConfig() :
        frameRate(48000), framesPerBurst(960) {}
};

struct ETDevice {
    virtual ~ETDevice() = default;
    virtual const DeviceAudioConfig* ET_getAudioConfig() const = 0;
};

#endif /* __ET_DEVICE_HPP__ */