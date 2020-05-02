#ifndef __AUDIO_MODULE_HPP__
#define __AUDIO_MODULE_HPP__

#include "Core/SystemModule.hpp"

class AudioModule : public SystemModule {
public:

    AudioModule();
    virtual ~AudioModule();

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __AUDIO_MODULE_HPP__ */