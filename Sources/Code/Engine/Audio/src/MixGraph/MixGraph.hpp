#ifndef __MIX_GRAPH_HPP__
#define __MIX_GRAPH_HPP__

#include "MixGraph/CombineNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/Resampler.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Audio/ETSoundManagers.hpp"
#include "Filters/RecursiveFilter.hpp"
#include "Filters/Equalizer.hpp"

#include <unordered_set>

class SoundProxy;

class MixGraph {
public:

    MixGraph();
    ~MixGraph();

    bool init(const MixConfig& newMixConfig);
    void mixBufferAndConvert(float* out);

    bool setSoundCmd(SoundProxy* soundProxy, ESoundCommand cmd);

    const MixConfig& getMixConfig() const;
    Resampler& getResampler();
    Buffer& getResampleBuffer();
    Buffer& getCombineBuffer();

    void setMasterVolume(float newVolume);
    void setEqualizer(ESoundGroup soundGroup, const EqualizerSetup& eqSetup);
    float getMasterVolume() const;

private:

    void startSound(SoundProxy& soundProxy);
    void stopSound(SoundProxy& soundProxy);
    void pauseSound(SoundProxy& soundProxy);
    void resumeSound(SoundProxy& soundProxy);

    void updatePendingStarts();
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
    std::unordered_set<SoundProxy*> pendingStarts;
    float masterVolume;
};

#endif /* __MIX_GRAPH_HPP__ */