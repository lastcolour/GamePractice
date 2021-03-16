#ifndef __AUDIO_MODULE_HPP__
#define __AUDIO_MODULE_HPP__

#include "Core/SystemModule.hpp"

class AudioModule : public SystemModule {
public:

    AudioModule();
    virtual ~AudioModule();

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void createSystemConfigs() const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __AUDIO_MODULE_HPP__ */