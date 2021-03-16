#ifndef __DESKTOP_MODULE_HPP__
#define __DESKTOP_MODULE_HPP__

#include "Core/SystemModule.hpp"

class DesktopModule : public SystemModule {
public:

    DesktopModule();
    virtual ~DesktopModule() = default;

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void createSystemConfigs() const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};


#endif /* __DESKTOP_MODULE_HPP__ */