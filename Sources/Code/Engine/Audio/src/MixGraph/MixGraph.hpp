#ifndef __MIX_GRAPH_HPP__
#define __MIX_GRAPH_HPP__

#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/Resampler.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Filters/RecursiveFilter.hpp"
#include "Filters/Equalizer.hpp"

class SoundProxy;

class MixGraph {
public:

    MixGraph();
    ~MixGraph();

    bool init(const MixConfig& newMixConfig);
    void mixBufferAndConvert(float* out);

    bool startSound(SoundProxy& soundProxy, float duration, bool isEvent);
    void stopSound(SoundProxy& soundProxy, float duration, bool resetOffset);

    const MixConfig& getMixConfig() const;
    Resampler& getResampler();
    Buffer& getResampleBuffer();
    Buffer& getCombineBuffer();

    void setMasterVolume(float newVolume);
    void setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup);
    float getMasterVolume() const;

    void applyLowPass(float* out, int channels, int samples);
    void resizeBuffers(int channels, int samples);
    MixNode* getFreeSource();
    CombineNode* getCombineNode(ESoundGroup soundGroup);

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