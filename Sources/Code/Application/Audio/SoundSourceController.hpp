#ifndef __SOUND_SOURCE_CONTROLLER_HPP__
#define __SOUND_SOURCE_CONTROLLER_HPP__

class OggDataStream;

struct SoundSourceController {
    virtual ~SoundSourceController() = default;
    virtual void detachFromSource() = 0;
    virtual OggDataStream* getDataStream() = 0;
};

#endif /* __SOUND_SOURCE_CONTROLLER_HPP__ */