#ifndef __OGG_SOURCE_NODE_HPP__
#define __OGG_SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggDataStream.hpp"

class SoundProxy;

class OggSourceNode : public MixNode {
public:

    OggSourceNode(MixGraph* mixGraph);
    virtual ~OggSourceNode();

    bool setSound(SoundProxy* proxy);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    OggDataStream oggData;
    SoundProxy* soundProxy;
};

#endif /* __OGG_SOURCE_NODE_HPP__ */