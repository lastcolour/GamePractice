#ifndef __OGG_DATA_STREAM_HPP__
#define __OGG_DATA_STREAM_HPP__

#include "Core/Buffer.hpp"

struct stb_vorbis;

class OggDataStream {
public:

    OggDataStream();
    ~OggDataStream();

    void setSampleOffset(int sampleOffset);

    bool open(Buffer& buffer);
    void close();
    bool isOpened() const;

    int readI16(void* out, int channels, int samples, bool looped);
    int readF32(void* out, int channels, int samples, bool looped);

    int getChannels() const;
    int getSampleRate() const;

private:

    Buffer oggBuffer;
    stb_vorbis* oggStream;
    int oggChannels;
    int oggSampleRate;
    int oggSampleCount;
};

#endif /* __OGG_DATA_STREAM_HPP__ */