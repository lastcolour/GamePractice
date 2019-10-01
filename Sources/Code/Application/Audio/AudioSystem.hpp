#ifndef __AUDIO_SYSTEM_HPP__
#define __AUDIO_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"

class AudioSystem : public SystemLogic {
public:

    AudioSystem();
    virtual ~AudioSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;
};

#endif /* __AUDIO_SYSTEM_HPP__ */