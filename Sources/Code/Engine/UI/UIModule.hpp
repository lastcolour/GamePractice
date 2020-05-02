#ifndef __UI_MODULE_HPP__
#define __UI_MODULE_HPP__

#include "Core/SystemModule.hpp"

class UIModule : public SystemModule {
public:

    UIModule();
    virtual ~UIModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __UI_MODULE_HPP__ */