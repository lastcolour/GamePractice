#ifndef __SOUND_PROXY_HPP__
#define __SOUND_PROXY_HPP__

#include "SoundData.hpp"

#include <atomic>

class Sound;
class MixNode;
enum class ESoundGroup;

class SoundProxy {
public:

    SoundProxy();
    ~SoundProxy();

    ESoundGroup readGroup() const;
    float readVolume() const;
    bool readLooped() const;

    void writeGroup(ESoundGroup newGroup);
    void writeVolume(float newVolume);
    void writeLooped(bool flag);

    void setFile(const char* fileName);
    const char* getFile() const;
    void setData(std::shared_ptr<SoundData>& newData);
    std::shared_ptr<SoundData>& getData();
    void setOffset(int newOffset);
    int getOffset() const;
    void setMixNode(MixNode* node);
    MixNode* getMixNode();
    void setKeepLoaded(bool flag);

    void setNoSound();

    void emit();
    void fadeInPlay(const Sound& sound, float duration);
    void fadeOutStop(float duration);
    void play(const Sound& sound);
    void pause();
    void resume(const Sound& sound);
    void stop();
    bool isPlaying() const;
    void syncPlayParams(const Sound& sound);

private:

    SoundProxy(const SoundProxy&) = delete;
    SoundProxy& operator=(const SoundProxy&) = delete;

private:

    std::shared_ptr<SoundData> soundData;
    std::atomic<MixNode*> mixNode;
    int offset;
    float volume;
    ESoundGroup group;
    bool looped;
    std::atomic<bool> keepInMemory;
};

#endif /* __SOUND_PROXY_HPP__ */