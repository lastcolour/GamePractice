#ifndef __OBOE_MIXER_HPP__
#define __OBOE_MIXER_HPP__

#include "Audio/Resampler.hpp"

#include <oboe/Oboe.h>

class OboeSoundSource;

class OboeMixer {
public:

    OboeMixer();
    ~OboeMixer();

    void setOutRate(int deviceOutRate);

    void startMixing(int outChannels, oboe::AudioFormat outFormat, void* outBuffer, int outNumFrames);
    void addSource(OboeSoundSource& soundSource);
    void endMixing();

private:

    struct MixBuffer {
        std::unique_ptr<float[]> data;
        int size;

        MixBuffer() : data(nullptr), size(0) {}
        void updateSize(int newSize);
    };

private:

    void mixSilence(MixBuffer& buffer);
    void fillSourceBuffer(MixBuffer& sourceBuffer, OboeSoundSource& source, int readFramesCount);
    void resampleSourceBuffer(MixBuffer& sourceBuffer, OboeSoundSource& source, int inSamplesCount);
    void postProcessSourceBuffer(MixBuffer& sourceBuffer,OboeSoundSource& source);
    void mergeSourceToMixBuffers(MixBuffer& sourceBuffer, MixBuffer& mixBuffer);

private:

private:

    Resampler resampler;
    MixBuffer sourceBuffer;
    MixBuffer resampleBuffer;
    MixBuffer mixBuffer;
    void* buffer;
    oboe::AudioFormat format;
    int channels;
    int numFrames;
};

#endif /* __OBOE_MIXER_HPP__ */