#ifndef __OGG_SOURCE_NODE_HPP__
#define __OGG_SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggDataStream.hpp"
#include "MixGraph/SoundSource.hpp"
#include "Fader.hpp"

class SoundProxy;

class OggSourceNode : public MixNode {
public:

    OggSourceNode(MixGraph* mixGraph);
    virtual ~OggSourceNode();

    bool setSound(SoundProxy* newSoundProxy, bool isEvent);
    Fader& getFader();
    void setResetOffsetOnStop(bool flag);

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    Fader fader;
    OggDataStream oggData;
    SoundSource soundSource;
};

#endif /* __OGG_SOURCE_NODE_HPP__ */