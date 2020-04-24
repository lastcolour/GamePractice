#ifndef __AUDIO_CONFIG_HPP__
#define __AUDIO_CONFIG_HPP__

class JSONNode;

class AudioConfig {
public:

    int maxSoundSources;

public:

    AudioConfig();
    ~AudioConfig();

    void serialize(const JSONNode& node);
};

#endif /* __AUDIO_CONFIG_HPP__ */