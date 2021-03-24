#ifndef __OGG_SOURCE_NODE_HPP__
#define __OGG_SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggDataStream.hpp"
#include "Fader.hpp"

class SoundProxy;

class OggSourceNode : public MixNode {
public:

    OggSourceNode(MixGraph* mixGraph);
    virtual ~OggSourceNode();

    bool setSound(SoundProxy* proxy);
    Fader& getFader();
    void setResetOffsetOnStop(bool flag);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    Fader fader;
    OggDataStream oggData;
    SoundProxy* soundProxy;
    bool resetOffsetOnStop;
};

#endif /* __OGG_SOURCE_NODE_HPP__ */