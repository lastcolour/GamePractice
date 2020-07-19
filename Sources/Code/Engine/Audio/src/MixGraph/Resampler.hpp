#ifndef __RESAMPLER_HPP__
#define __RESAMPLER_HPP__

#include "Core/Buffer.hpp"

class OggDataStream;
class MixGraph;

struct StreamMixState {
    int samplesRead;
    bool isEnded;
};

class Resampler {
public:

    Resampler(MixGraph* mixGraph);
    ~Resampler();

    StreamMixState exclusiveResampleTo(float* out, int channels, int samples, bool looped, OggDataStream* stream);

private:

    MixGraph* graph;
    Buffer buffer;
};

#endif /* __RESAMPLER_HPP__ */