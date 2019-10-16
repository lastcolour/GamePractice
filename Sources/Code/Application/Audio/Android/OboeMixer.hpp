#ifndef __OBOE_MIXER_HPP__
#define __OBOE_MIXER_HPP__

#include <oboe/Oboe.h>

class OboeSoundSource;

class OboeMixer {
public:

    OboeMixer();
    ~OboeMixer();

    void startMixing(int outChannels, oboe::AudioFormat outFormat, void* outBuffer, int outNumFrames);
    void addSource(OboeSoundSource& soundSource);
    void endMixing();

private:

    void mixSilence();
    void adjustMixBuffer();

private:

    std::unique_ptr<float[]> mixBuffer;
    int mixBufferFrames;
    void* buffer;
    oboe::AudioFormat format;
    int channels;
    int numFrames;
};

#endif /* __OBOE_MIXER_HPP__ */