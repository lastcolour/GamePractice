#ifndef __SOUND_STREAM_HPP__
#define __SOUND_STREAM_HPP__

#include <memory>

class SourceNode;
class Buffer;

class SoundStream {
public:
    virtual ~SoundStream() = default;
    virtual unsigned int getSamplesOffset() const = 0;
    virtual void setMixNode(SourceNode* node) = 0;
    virtual float getMixVolume() const = 0;
    virtual bool isEvent() const = 0;
    virtual bool isMixLooped() const = 0;
    virtual Buffer& getData() = 0;
};

#endif /* __SOUND_STREAM_HPP__ */