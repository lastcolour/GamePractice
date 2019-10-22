#ifndef __OGG_DATA_STREAM_HPP__
#define __OGG_DATA_STREAM_HPP__

#include "Core/Buffer.hpp"

struct stb_vorbis;
class Sound;

class OggDataStream {
public:

    OggDataStream(const Buffer& buffer);
    ~OggDataStream();

    bool isOpened() const;

    int fillF32(void* outData, int samplesCount, int channels, bool looped);
    int fillI16(void* outDAta, int samplesCount, int channels, bool looped);

    int readF32(void* outData, int samplesCount, int channels);
    int readI16(void* outData, int samplesCount, int channels);

    void setSampleOffset(int sampleOffset);

public:

    Buffer oggBuffer;
    stb_vorbis* oggStream;
    Sound* sound;
    int channels;
    int sampleRate;
    int numSamples;
};

#endif /* __OGG_DATA_STREAM_HPP__ */