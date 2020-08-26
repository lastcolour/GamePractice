#ifndef __SOURCE_RAMP_NODE_HPP__
#define __SOURCE_RAMP_NODE_HPP__

#include "MixGraph/SourceNode.hpp"
#include "MixGraph/MixNode.hpp"
#include "MixGraph/OggSourceNode.hpp"

class SourceRampNode : public MixNode, public SourceNode {
public:

    SourceRampNode();
    virtual ~SourceRampNode();

    // SourceNode
    void detachFromStream() override;
    void attachToStream(SoundStream* stream) override;

    // MixNode
    void additiveMixTo(float* out, int channels, int samples) override;

private:

    void attachToGraph();
    void detachFromGraph();

private:

    enum class State {
        FadeIn = 0,
        Normal,
        FadeOut
    };

private:

    OggSourceNode oggSource;
    State state;
    int fadeintEnd;
    int fadeoutStart;
    int offset;
    int maxSamplesToProcess;
    float fraction;
    float value;
};

#endif /* __SOURCE_RAMP_NODE_HPP__ */