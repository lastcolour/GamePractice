#ifndef __ENTITY_MODULE_HPP__
#define __ENTITY_MODULE_HPP__

#include "Core/SystemModule.hpp"

class EntityModule : public SystemModule {
public:

    EntityModule();
    virtual ~EntityModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __ENTITY_MODULE_HPP__ */