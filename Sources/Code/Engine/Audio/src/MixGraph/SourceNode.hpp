#ifndef __SOURCE_NODE_HPP__
#define __SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "OggDataStream.hpp"

#include <memory>

class SoundStream;
class OggDataStream;

class SourceNode : public MixNode {
public:

    SourceNode();
    virtual ~SourceNode();

    void detachFromStream();
    void attachToStream(SoundStream* stream);
    unsigned int getSamplesOffset() const;

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    OggDataStream oggData;
    SoundStream* soundStream;
    unsigned int samplesOffset;
    float volume;
};

#endif /* __SOURCE_NODE_HPP__ */