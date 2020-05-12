#ifndef __AUDIO_MODULE_HPP__
#define __AUDIO_MODULE_HPP__

#include "Core/SystemModule.hpp"

class AudioModule : public SystemModule {
public:

    AudioModule();
    virtual ~AudioModule();

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void reflectSystemConfigs(ReflectContext& ctx) const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __AUDIO_MODULE_HPP__ */