#ifndef __SOURCE_NODE_HPP__
#define __SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"

class SoundStream;

class SourceNode : public MixNode {
public:

    SourceNode();
    virtual ~SourceNode();

    void setStream(SoundStream* stream);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

    void detachFromSource();

private:

    SoundStream* soundStream;
};

#endif /* __SOURCE_NODE_HPP__ */