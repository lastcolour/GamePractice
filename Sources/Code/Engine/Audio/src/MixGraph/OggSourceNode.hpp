#ifndef __OGG_SOURCE_NODE_HPP__
#define __OGG_SOURCE_NODE_HPP__

#include "MixGraph/MixNode.hpp"
#include "OggDataStream.hpp"

#include <memory>

class SoundStream;
class OggDataStream;

class OggSourceNode : public MixNode {
public:

    OggSourceNode();
    virtual ~OggSourceNode();

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;
    void exclusiveMixTo(float* out, int channels, int samples);

    void openStream(SoundStream* stream);
    void closeStream();

    unsigned int getSamplesOffset() const;
    unsigned int getSampleRate() const;
    unsigned int getTotalSamples() const;
    SoundStream* getSoundStream();
    void setSoundStream(SoundStream* newStream);

private:

    OggDataStream oggData;
    SoundStream* soundStream;
    unsigned int samplesOffset;
    float volume;
};

#endif /* __OGG_SOURCE_NODE_HPP__ */