#ifndef __RESAMPLERS_HPP__
#define __RESAMPLERS_HPP__

#include <cstdint>

class BitIndexer {
public:

    struct IndexLerp {
        int idx;
        float t;
    };

public:

    BitIndexer(int inRate, int outRate) :
        index(0),
        frac((inRate << 15) / outRate) {}

    void restart() {
        index = 0;
    }

    IndexLerp next() {
        IndexLerp res;
        res.idx = index >> 15;
        res.t = (index - (res.idx << 15)) / static_cast<float>(1 << 15);
        index += frac;
        return res;
    }

private:

    uint32_t index;
    uint32_t frac;
};

class LinearResampler {
public:

    LinearResampler(int inRate, int outRate) :
        indexer(inRate, outRate),
        lprev(0.f),
        rprev(0.f),
        initialized(false) {
    }

    void resampleMono(float* from, float* to, int samples) {
        if(!initialized) {
            initialized = true;
            lprev = 0.f;
            rprev = 0.f;
        }
        indexer.restart();
        for(int i = 0; i < samples; ++i) {
            auto next = indexer.next();
            to[i] = Math::Lerp(lprev, from[next.idx], next.t);
            lprev = from[next.idx];
        }
    }

    void resampleStereo(float* from, float* to, int samples) {
        if(!initialized) {
            initialized = true;
            lprev = 0.f;
            rprev = 0.f;
        }
        indexer.restart();
        for(int i = 0; i < samples; ++i) {
            auto next = indexer.next();
            to[2 * i] = Math::Lerp(lprev, from[2 * next.idx], next.t);
            to[2 * i + 1] = Math::Lerp(rprev, from[2 * next.idx + 1], next.t);
            lprev = from[2 * next.idx];
            rprev = from[2 * next.idx + 1];
        }
    }

private:

    BitIndexer indexer;
    float lprev;
    float rprev;
    bool initialized;
};

#endif /* __RESAMPLERS_HPP__ */