#ifndef __ENTITY_MODULE_HPP__
#define __ENTITY_MODULE_HPP__

#include "Core/SystemModule.hpp"

class EntityModule : public SystemModule {
public:

    EntityModule();
    virtual ~EntityModule() = default;

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void createSystemConfigs() const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __ENTITY_MODULE_HPP__ */