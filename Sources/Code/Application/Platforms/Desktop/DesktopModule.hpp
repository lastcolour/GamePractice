#ifndef __DESKTOP_MODULE_HPP__
#define __DESKTOP_MODULE_HPP__

#include "Core/SystemModule.hpp"

class DesktopModule : public SystemModule {
public:

    DesktopModule();
    virtual ~DesktopModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
};


#endif /* __DESKTOP_MODULE_HPP__ */