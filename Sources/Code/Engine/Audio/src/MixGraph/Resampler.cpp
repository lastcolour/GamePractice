#include "Resampler.hpp"
#include "SoundStream.hpp"
#include "MixGraph/MixGraph.hpp"
#include "Math/Primitivies.hpp"
#include "OggDataStream.hpp"

#include <cassert>

Resampler::Resampler(MixGraph* mixGraph) :
    graph(mixGraph) {
}

Resampler::~Resampler() {
}

StreamMixState Resampler::exclusiveResampleTo(float* out, int channels, int samples, bool looped, OggDataStream* stream) {
    auto inSampleRate =  stream->getSampleRate();
    auto outSampleRate = graph->getMixConfig().outSampleRate;

    StreamMixState mixState;
    if(inSampleRate == outSampleRate) {
        mixState.samplesRead = stream->readF32(out, channels, samples, looped);
        mixState.isEnded = mixState.samplesRead < samples;
        return mixState;
    }

    auto inSamples = (outSampleRate * inSampleRate) / samples;
    auto inBufferSize = inSamples * channels * sizeof(float);
    if(buffer.getSize() < inBufferSize) {
        buffer.resize(inBufferSize);
    }

    float* source = static_cast<float*>(buffer.getWriteData());
    std::fill_n(source, channels * inSamples, 0.f);

    mixState.samplesRead = stream->readF32(source, channels, inSamples, looped);
    mixState.isEnded = mixState.samplesRead < inSamples;

    float ratio = inSamples / static_cast<float>(samples);
    float currentFIdx = 0.f;

    if(channels == 1) {
        for(int i = 0; i < samples; ++i) {
            auto startIdx = static_cast<int>(currentFIdx);
            auto endIdx = startIdx + 1;
            auto prog = currentFIdx - startIdx;
            out[i] = Math::Lerp(source[startIdx], source[endIdx], prog);
            currentFIdx += ratio;
        }
    } else if (channels == 2) {
        for(int i = 0; i < samples; ++i) {
            auto startIdx = static_cast<int>(currentFIdx);
            auto endIdx = startIdx + 1;
            auto prog = currentFIdx - startIdx;
            out[2 * i] = Math::Lerp(source[2 * startIdx], source[2 * endIdx], prog);
            out[2 * i + 1] = Math::Lerp(source[2 * startIdx + 1], source[2 * endIdx + 1], prog);
            currentFIdx += ratio;
        }
    } else {
        assert(false && "Too many channels");
    }

    return mixState;
}