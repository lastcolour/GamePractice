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
    const char* readFile() const;

    void writeFile(const char* fileName);
    void writeGroup(ESoundGroup newGroup);
    void writeVolume(float newVolume);
    void writeLooped(bool flag);

    void setData(std::shared_ptr<SoundData>& newData);
    std::shared_ptr<SoundData>& getData();
    void setOffset(int newOffset);
    int getOffset() const;
    void setMixNode(MixNode* node);
    MixNode* getMixNode();

    void setNoSound();

    void fadeInPlay(const Sound&, float duration);
    void fadeOutStop(float duration);
    void play(const Sound& sound);
    void pause();
    void resume(const Sound& sound);
    void stop();
    bool isPlaying() const;
    bool isValid() const;
    bool canRemove() const;

private:

    SoundProxy(const SoundProxy&) = delete;
    SoundProxy& operator=(const SoundProxy&) = delete;

private:

    std::shared_ptr<SoundData> data;
    MixNode* mixNode;
    int offset;
    std::atomic<float> volume;
    std::atomic<ESoundGroup> group;
    std::atomic<bool> looped;
    std::atomic<bool> hasSound;
    std::atomic<bool> hasMixNode;
};

#endif /* __SOUND_PROXY_HPP__ */