#include "Fader.hpp"

#include <cassert>

namespace {

int16_t multByFractionInt16(int32_t x, int32_t a, int32_t b) {
    return static_cast<int16_t>(x * a / b);
}

} // namespace

Fader::Fader() :
    state(State::FadeInDone),
    samplesDone(0),
    totalSamples(0) {
}

Fader::~Fader() {
}

void Fader::setFadeIn(int samplesDuration) {
    assert(samplesDuration >= 0 && "Invalid samples count");
    if(samplesDuration == 0) {
        state = State::FadeInDone;
        samplesDone = 0;
        totalSamples = 0;
        return;
    }
    switch(state) {
        case State::FadeIn: {
            assert(totalSamples > 0 && "Invalid total samples");
            samplesDone = (samplesDone * samplesDuration) / totalSamples;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeInDone: {
            state = State::FadeIn;
            samplesDone = 0;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeOut: {
            assert(totalSamples > 0 && "Invalid total samples");
            state = State::FadeIn;
            samplesDone = (samplesDone * samplesDuration) / totalSamples;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeOutDone: {
            state = State::FadeIn;
            samplesDone = 0;
            totalSamples = samplesDuration;
            break;
        }
        default: {
            assert(false && "Invalid state");
        }
    }
}

void Fader::setFadeOut(int samplesDuration) {
    assert(samplesDuration >= 0 && "Invalid samples count");
    if(samplesDuration == 0) {
        state = State::FadeOutDone;
        samplesDone = 0;
        totalSamples = 0;
        return;
    }
    switch(state) {
        case State::FadeIn: {
            assert(totalSamples > 0 && "Invalid total samples");
            state = State::FadeOut;
            samplesDone = (samplesDone * samplesDuration) / totalSamples;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeInDone: {
            state = State::FadeOut;
            samplesDone = 0;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeOut: {
            assert(totalSamples > 0 && "Invalid total samples");
            samplesDone = (samplesDone * samplesDuration) / totalSamples;
            totalSamples = samplesDuration;
            break;
        }
        case State::FadeOutDone: {
            state = State::FadeOut;
            samplesDone = 0;
            totalSamples = samplesDuration;
            break;
        }
        default: {
            assert(false && "Invalid state");
        }
    }
}

bool Fader::isFadeInState() const {
    return state == State::FadeIn || state == State::FadeInDone;
}

bool Fader::isFadeOutState() const {
    return state == State::FadeOut || state == State::FadeOutDone;
}

bool Fader::isFadeOutDone() const {
    return state == State::FadeOutDone;
}

void Fader::exclusiveTransformFloat(float* data, int channels, int samples) {
    int fadeOutOffset = 0;
    switch(state) {
        case State::FadeIn: {
            int maxSamplesToProcess = std::min(totalSamples - samplesDone, samples);
            if(channels == 1) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    float volume = samplesDone / static_cast<float>(totalSamples);
                    data[i] *= volume;
                    ++samplesDone;
                }
            } else if(channels == 2) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    float volume = samplesDone / static_cast<float>(totalSamples);
                    data[2 * i] *= volume;
                    data[2 * i + 1] *= volume;
                    ++samplesDone;
                }
            }
            assert(samplesDone <= totalSamples && "Samples overflow");
            if(samplesDone == totalSamples) {
                state = State::FadeInDone;
            }
            break;
        }
        case State::FadeInDone: {
            break;
        }
        case State::FadeOut: {
            int maxSamplesToProcess = std::min(totalSamples - samplesDone, samples);
            if(channels == 1) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    float volume = 1.f - samplesDone / static_cast<float>(totalSamples);
                    data[i] *= volume;
                    ++samplesDone;
                }
            } else if(channels == 2) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    float volume = 1.f - samplesDone / static_cast<float>(totalSamples);
                    data[2 * i] *= volume;
                    data[2 * i + 1] *= volume;
                    ++samplesDone;
                }
            }
            assert(samplesDone <= totalSamples && "Samples overflow");
            int remainingSamples = samples - maxSamplesToProcess;
            if(remainingSamples == 0) {
                break;
            }
            fadeOutOffset = samplesDone;
            state = State::FadeOutDone;
            [[fallthrough]];
        }
        case State::FadeOutDone: {
            if(channels == 1) {
                for(int i = fadeOutOffset; i < samples; ++i) {
                    data[i] = 0.f;
                }
            } else if(channels == 2) {
                for(int i = fadeOutOffset; i < samples; ++i) {
                    data[2 * i] = 0.f;
                    data[2 * i + 1] = 0.f;
                }
            }
            break;
        }
        default: {
            assert(false && "Invalid state");
        }
    }
}

void Fader::exclusiveTransformInt16(int16_t* data, int channels, int samples) {
    int fadeOutOffset = 0;
    switch(state) {
        case State::FadeIn: {
            int maxSamplesToProcess = std::min(totalSamples - samplesDone, samples);
            if(channels == 1) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    data[i] = multByFractionInt16(data[i], samplesDone, totalSamples);
                    ++samplesDone;
                }
            } else if(channels == 2) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    data[2 * i] = multByFractionInt16(data[2 * i], samplesDone, totalSamples);
                    data[2 * i + 1] = multByFractionInt16(data[2 * i + 1], samplesDone, totalSamples);
                    ++samplesDone;
                }
            }
            assert(samplesDone <= totalSamples && "Samples overflow");
            if(samplesDone == totalSamples) {
                state = State::FadeInDone;
            }
            break;
        }
        case State::FadeInDone: {
            break;
        }
        case State::FadeOut: {
            int maxSamplesToProcess = std::min(totalSamples - samplesDone, samples);
            if(channels == 1) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    data[i] = multByFractionInt16(data[i], totalSamples - samplesDone, totalSamples);
                    ++samplesDone;
                }
            } else if(channels == 2) {
                for(int i = 0; i < maxSamplesToProcess; ++i) {
                    data[2 * i] = multByFractionInt16(data[2 * i], totalSamples - samplesDone, totalSamples);
                    data[2 * i + 1] = multByFractionInt16(data[2 * i + 1], totalSamples - samplesDone, totalSamples);
                    ++samplesDone;
                }
            }
            assert(samplesDone <= totalSamples && "Samples overflow");
            int remainingSamples = samples - maxSamplesToProcess;
            if(remainingSamples == 0) {
                break;
            }
            fadeOutOffset = samplesDone;
            state = State::FadeOutDone;
            [[fallthrough]];
        }
        case State::FadeOutDone: {
            if(channels == 1) {
                for(int i = fadeOutOffset; i < samples; ++i) {
                    data[i] = 0;
                }
            } else if(channels == 2) {
                for(int i = fadeOutOffset; i < samples; ++i) {
                    data[2 * i] = 0;
                    data[2 * i + 1] = 0;
                }
            }
            break;
        }
        default: {
            assert(false && "Invalid state");
        }
    }
}

void Fader::reset() {
    state = State::FadeInDone;
    samplesDone = 0;
    totalSamples = 0;
}