#ifndef __PLATFORM_MODULE_HPP__
#define __PLATFORM_MODULE_HPP__

#include "Core/SystemModule.hpp"

class PlatformModule : public SystemModule {
public:

    PlatformModule();
    virtual ~PlatformModule() = default;

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void createSystemConfigs() const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __PLATFORM_MODULE_HPP__ */