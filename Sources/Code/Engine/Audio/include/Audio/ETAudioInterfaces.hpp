#ifndef __ET_AUDIO_INTERFACES_HPP__
#define __ET_AUDIO_INTERFACES_HPP__

#include "Core/ETPrimitives.hpp"

#include <memory>

class Sound;
struct SoundSource;

struct ETSoundPlayer {
    virtual ~ETSoundPlayer() = default;
    virtual void ET_setSound(const char* newSoundName) = 0;
    virtual void ET_play() = 0;
    virtual void ET_pause() = 0;
    virtual void ET_resume() = 0;
    virtual void ET_stop() = 0;
};

struct ETSoundManager {
    virtual ~ETSoundManager() = default;
    virtual std::unique_ptr<Sound> ET_createSound(const char* soundName) = 0;
};

struct ETSoundSourceManager {
    virtual ~ETSoundSourceManager() = default;
    virtual SoundSource* ET_getFreeSource() = 0;
    virtual void ET_returnSoundSource(SoundSource* retSoundSource) = 0;
};

struct ETAudioSystem {
    virtual ~ETAudioSystem() = default;
    virtual std::vector<SoundSource*> ET_getSourcesToManage() = 0;
};

#endif /* __ET_AUDIO_INTERFACES_HPP__ */