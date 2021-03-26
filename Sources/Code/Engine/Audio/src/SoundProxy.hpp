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

    bool shouldStartMix() const;
    bool isLoaded() const;
    void setPendingStart(bool flag);
    void setNoSound();

    void updateDataLoadState();

    void emit();
    void fadeInPlay(const Sound&, float duration);
    void fadeOutStop(float duration);
    void play(const Sound& sound);
    void pause();
    void resume(const Sound& sound);
    void stop();
    bool isPlaying() const;
    bool canRemove() const;

private:

    SoundProxy(const SoundProxy&) = delete;
    SoundProxy& operator=(const SoundProxy&) = delete;

private:

    std::shared_ptr<SoundData> soundData;
    MixNode* mixNode;
    int offset;
    std::atomic<float> volume;
    std::atomic<ESoundGroup> group;
    std::atomic<bool> looped;
    std::atomic<bool> hasSound;
    std::atomic<bool> hasMixNode;
    std::atomic<bool> keepInMemory;
    bool isQueuedToStartMix;
};

#endif /* __SOUND_PROXY_HPP__ */