#ifndef __MIX_GRAPH_HPP__
#define __MIX_GRAPH_HPP__

#include "MixGraph/CombineNode.hpp"
#include "MixGraph/SourceNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/Resampler.hpp"
#include "Core/Buffer.hpp"
#include "Filters/Equalizer.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Filters/RecursiveFilter.hpp"

#include <vector>
#include <memory>

class SoundStream;

class MixGraph {
public:

    MixGraph();
    ~MixGraph();

    bool init(const MixConfig& newMixConfig);
    void mixBufferAndConvert(float* out);

    bool playSound(SoundStream* stream);

    const MixConfig& getMixConfig() const;
    Resampler& getResampler();
    Buffer& getResampleBuffer();
    Buffer& getCombineBuffer();

    void setMasterVolume(float newVolume);
    void setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup);
    float getMasterVolume() const;

private:

    void applyLowPass(float* out, int channels, int samples);
    void resizeBuffers(int channels, int samples);
    MixNode* getFreeSource();

private:

    MixConfig config;
    Resampler resampler;
    CombineNode gameNode;
    CombineNode musicNode;
    CombineNode uiNode;
    Buffer resampleBuffer;
    Buffer combineBuffer;
    RecursiveFilter lLowPass;
    RecursiveFilter rLowPass;
    std::vector<std::unique_ptr<MixNode>> sources;
    float masterVolume;
};

#endif /* __MIX_GRAPH_HPP__ */