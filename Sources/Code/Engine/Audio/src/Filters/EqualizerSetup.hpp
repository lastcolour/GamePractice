#ifndef __EQUALIZER_SETUP_HPP__
#define __EQUALIZER_SETUP_HPP__

class EqualizerSetup {
public:

    static void Reflect(ReflectContext& ctx);

public:

    EqualizerSetup();

public:

    float volume;
    float low;
    float lowMid;
    float mid;
    float highMid;
    float high;
};

#endif /* __EQUALIZER_SETUP_HPP__ */