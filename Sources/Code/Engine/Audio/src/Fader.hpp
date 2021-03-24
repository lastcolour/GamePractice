#ifndef __FADER_HPP__
#define __FADER_HPP__

#include <cstdint>

class Fader {
public:

    Fader();
    ~Fader();

    void setFadeIn(int samplesDuration);
    void setFadeOut(int samplesDuration);
    void reset();

    bool isFadeInState() const;
    bool isFadeOutState() const;
    bool isFadeOutDone() const;

    void exclusiveTransformFloat(float* data, int channels, int samples);
    void exclusiveTransformInt16(int16_t* data, int channels, int samples);

private:

    enum class State {
        FadeIn,
        FadeInDone,
        FadeOut,
        FadeOutDone,
    };

private:

    State state;
    int samplesDone;
    int totalSamples;
};

#endif /* __FADER_HPP__ */