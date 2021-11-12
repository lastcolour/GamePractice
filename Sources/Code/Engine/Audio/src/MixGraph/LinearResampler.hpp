#ifndef __RESAMPLERS_HPP__
#define __RESAMPLERS_HPP__

class LinearResampler {
public:

    LinearResampler(int inSampleRate, int outSampleRate) :
        inRate(inSampleRate),
        outRate(outSampleRate),
        fIndex(0),
        frac(0),
        lprev(0.f),
        rprev(0.f) {

        frac = (inRate << 15) / outRate;
        fIndex = frac;
    }

    void resampleMono(float* from, int fromLenght, float* to, int toLenght) {
        int j = 0;
        while(fIndex < (1 << 15)) {
            int i = fIndex >> 15;
            float t = (fIndex - (i << 15)) / static_cast<float>(1 << 15);

            to[j] = Math::Lerp(lprev, from[0], t);

            fIndex += frac;
            ++j;
        }
        while(j < toLenght) {
            int i = fIndex >> 15;
            float t = (fIndex - (i << 15)) / static_cast<float>(1 << 15);

            to[j] = Math::Lerp(from[i - 1], from[std::min(i, fromLenght - 1)], t);

            fIndex += frac;
            ++j;
        }

        fIndex -= ((fIndex - frac) >> 15) << 15;
        lprev = from[fromLenght - 1];
    }

    void resampleStereo(float* from, int fromLenght, float* to, int toLenght) {
        int j = 0;
        while(fIndex < (1 << 15)) {
            int i = fIndex >> 15;
            float t = (fIndex - (i << 15)) / static_cast<float>(1 << 15);

            to[2 * j] = Math::Lerp(lprev, from[0], t);
            to[2 * j + 1] = Math::Lerp(rprev, from[1], t);

            fIndex += frac;
            ++j;
        }
        while(j < toLenght) {
            int i = fIndex >> 15;
            int k = std::min(i, fromLenght - 1);
            float t = (fIndex - (i << 15)) / static_cast<float>(1 << 15);

            to[2 * j] = Math::Lerp(from[2 * (i - 1)], from[2 * k], t);
            to[2 * j + 1] = Math::Lerp(from[2 * (i - 1) + 1], from[2 * k + 1], t);

            fIndex += frac;
            ++j;
        }

        fIndex -= ((fIndex - frac) >> 15) << 15;
        lprev = from[2 * (fromLenght - 1)];
        rprev = from[2 * (fromLenght - 1) + 1];
    }

private:

    uint32_t inRate;
    uint32_t outRate;
    uint32_t fIndex;
    uint32_t frac;
    float lprev;
    float rprev;
};

#endif /* __RESAMPLERS_HPP__ */