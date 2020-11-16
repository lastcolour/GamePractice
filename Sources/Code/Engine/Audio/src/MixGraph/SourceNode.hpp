#ifndef __SOURCE_NODE_HPP__
#define __SOURCE_NODE_HPP__

class SoundStream;

struct SourceNode {
    virtual ~SourceNode() = default;
    virtual bool attachToStream(SoundStream* stream) = 0;
    virtual void detachFromStream() = 0;
};

#endif /* __SOURCE_NODE_HPP__ */