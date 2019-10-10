#ifndef __CORE_MODULE_HPP__
#define __CORE_MODULE_HPP__

#include "Core/SystemModule.hpp"

class CoreModule : public SystemModule {
public:

    CoreModule();
    virtual ~CoreModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
};

#endif /* __CORE_MODULE_HPP__ */