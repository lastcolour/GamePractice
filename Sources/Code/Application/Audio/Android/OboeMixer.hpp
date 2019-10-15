#ifndef __OBOE_MIXER_HPP__
#define __OBOE_MIXER_HPP__

class OboeSoundSource;

class OboeMixer {
public:

    OboeMixer();
    ~OboeMixer();

    void mixSilence();
    void mixSource(OboeSoundSource& soundSource);

private:

};

#endif /* __OBOE_MIXER_HPP__ */