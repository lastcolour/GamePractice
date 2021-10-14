#include "Resampler.hpp"
#include "MixGraph/MixGraph.hpp"
#include "OggDataStream.hpp"
#include "MixGraph/LinearResampler.hpp"
#include "SoundConfig.hpp"

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

    assert((samples * inSampleRate) % outSampleRate == 0 && "Invalid ration; Can't perfrom transform");

    int inSamples = samples * inSampleRate / outSampleRate;

    auto inBufferSize = inSamples * channels * sizeof(float);
    if(buffer.getSize() < inBufferSize) {
        buffer.resize(inBufferSize);
    }

    float* source = static_cast<float*>(buffer.getWriteData());
    std::fill_n(source, inSamples * channels, 0.f);

    mixState.samplesRead = stream->readF32(source, channels, inSamples, looped);
    mixState.isEnded = mixState.samplesRead < inSamples;

    auto resampler = stream->getResampler();
    if(!resampler) {
        stream->setResampler(new LinearResampler(inSampleRate, outSampleRate));
        resampler = stream->getResampler();
    }

    if(channels == 1) {
        resampler->resampleMono(source, mixState.samplesRead, out, samples);
    } else if(channels == 2) {
        resampler->resampleStereo(source, mixState.samplesRead, out, samples);
    } else {
        assert(false && "too many channels");
    }

    return mixState;
}