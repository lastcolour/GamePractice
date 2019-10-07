#ifndef __OGG_DATA_STREAM_HPP__
#define __OGG_DATA_STREAM_HPP__

#include "Core/Buffer.hpp"

struct stb_vorbis;

class OggDataStream {
public:

    OggDataStream(const Buffer& buffer);
    ~OggDataStream();

    bool isOpened() const;

public:

    stb_vorbis* oggStream;
    int channels;
    int sampleRate;
    int numSamples;
};

#endif /* __OGG_DATA_STREAM_HPP__ */