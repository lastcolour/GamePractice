#ifndef __MIX_GRAPH_HPP__
#define __MIX_GRAPH_HPP__

#include "MixGraph/CombineNode.hpp"
#include "MixGraph/SourceNode.hpp"
#include "MixGraph/MixConfig.hpp"
#include "MixGraph/Resampler.hpp"
#include "Core/Buffer.hpp"

#include <vector>
#include <memory>

class SoundStream;

class MixGraph {
public:

    MixGraph();
    ~MixGraph();

    bool init();
    void mix(float* out, int channels, int samples);

    bool playSound(SoundStream* stream);

    const MixConfig& getMixConfig() const;
    Resampler& getResampler();
    Buffer& getTempBuffer();

    void setMasterVolume(float newVolume);
    float getMasterVolume() const;

private:

    MixNode* getFreeSource();

private:

    Resampler resampler;
    MixConfig config;
    CombineNode rootCombine;
    Buffer buffer;
    std::vector<std::unique_ptr<MixNode>> sources;
    float masterVolume;
};

#endif /* __MIX_GRAPH_HPP__ */