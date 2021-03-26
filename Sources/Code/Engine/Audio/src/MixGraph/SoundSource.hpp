#ifndef __SOUND_SOURCE_HPP__
#define __SOUND_SOURCE_HPP__

class MixNode;
class SoundProxy;
class SoundData;

class SoundSource {
public:

    SoundSource();
    SoundSource(SoundProxy& soundProxy, bool isEventFlag);
    SoundSource(SoundSource&& other);
    SoundSource& operator=(SoundSource&& other);

    std::shared_ptr<SoundData>& getData();
    void setMixNode(MixNode* mixNode);
    MixNode* getMixNode();
    float getVolume() const;
    bool getLooped() const;
    int getOffset() const;
    void setOffset(int newOffset);
    void setResetOffset(bool flag);
    bool isNull() const;
    void reset();

private:

    SoundSource(const SoundSource&) = delete;
    SoundSource& operator=(const SoundSource&) = delete;

private:

    SoundProxy* proxy;
    bool isEvent;
    bool isRestOffset;
};

#endif /* __SOUND_SOURCE_HPP__ */