#ifndef __ANDROIND_MODULE_HPP__
#define __ANDROIND_MODULE_HPP__

#include "Core/SystemModule.hpp"

class AndroidModule : public SystemModule {
public:

    AndroidModule();
    virtual ~AndroidModule();

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void reflectSystemConfigs(ReflectContext& ctx) const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __ANDROIND_MODULE_HPP__ */