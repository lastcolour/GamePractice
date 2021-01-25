#ifndef __OGG_SOURCE_NODE_HPP__
#define __OGG_SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "OggDataStream.hpp"
#include "MixGraph/SourceNode.hpp"

#include <memory>

class SoundStream;
class OggDataStream;

class OggSourceNode : public MixNode, public SourceNode {
public:

    OggSourceNode(MixGraph* mixGraph);
    virtual ~OggSourceNode();

    // SourceNode
    bool attachToStream(SoundStream* stream) override;
    void detachFromStream() override;

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    OggDataStream oggData;
    SoundStream* soundStream;
    unsigned int samplesOffset;
    float volume;
};

#endif /* __OGG_SOURCE_NODE_HPP__ */