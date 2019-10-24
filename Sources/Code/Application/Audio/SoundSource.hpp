#ifndef __SOUND_SOURCE_HPP__
#define __SOUND_SOURCE_HPP__

class OggDataStream;

struct SoundSourceController {
    virtual ~SoundSourceController() = default;
    virtual void detachFromSource() = 0;
};

struct SoundSource {
    virtual ~SoundSource() = default;
    virtual void attachToDataStream(OggDataStream& newDataStream) = 0;
    virtual void stopStreaming() = 0;
    virtual void pauseStreaming() = 0;
    virtual void resumeStreaming() = 0;
    virtual void setGain(float newGain) = 0;
    virtual void setLoop(bool loopFlag) = 0;
    virtual bool isLooped() const = 0;
    virtual bool isPaused() const = 0;
    virtual void update() = 0;
};

#endif /* __SOUND_SOURCE_HPP__ */