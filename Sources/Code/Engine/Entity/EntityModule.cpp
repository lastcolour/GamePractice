#include "Entity/EntityModule.hpp"
#include "Entity/EntityManager.hpp"

EntityModule::EntityModule() :
    SystemModule("Entity") {}

EntityModule::LogicsContainerPtrT EntityModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            EntityManager>()
        );
    return container;
}

void EntityModule::reflectSystemConfigs(ReflectContext& ctx) const {
}

void EntityModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}