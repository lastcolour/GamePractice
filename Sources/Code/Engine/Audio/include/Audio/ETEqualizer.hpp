#ifndef __ET_EQUALIZER_HPP__
#define __ET_EQUALIZER_HPP__

struct ETEqualizer {
    virtual ~ETEqualizer() = default;
    virtual void ET_enable() = 0;
    virtual void ET_disable() = 0;
};

#endif /* __ET_EQUALIZER_HPP__ */