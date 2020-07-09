#ifndef __RESAMPLER_HPP__
#define __RESAMPLER_HPP__

#include "Core/Buffer.hpp"

class SoundStream;
class MixGraph;

class Resampler {
public:

    Resampler(MixGraph* mixGraph);
    ~Resampler();

    void exclusiveResampleTo(float* out, int channels, int samples, SoundStream& stream);

private:

    MixGraph* graph;
    Buffer buffer;
};

#endif /* __RESAMPLER_HPP__ */