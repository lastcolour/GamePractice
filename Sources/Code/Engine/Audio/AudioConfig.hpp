#ifndef __AUDIO_CONFIG_HPP__
#define __AUDIO_CONFIG_HPP__

class ReflectContext;

class AudioConfig {
public:

    static void Reflect(ReflectContext& ctx);

public:

    AudioConfig();
    ~AudioConfig();

public:

    int maxSoundSources;
};

#endif /* __AUDIO_CONFIG_HPP__ */