#include "Entity/EntityModule.hpp"
#include "EntityManager.hpp"
#include "AsyncEntityManager.hpp"

EntityModule::EntityModule() :
    SystemModule("Entity") {}

EntityModule::LogicsContainerPtrT EntityModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            EntityManager,
            AsyncEntityManager>()
        );
    return container;
}

void EntityModule::createSystemConfigs() const {
}

void EntityModule::registerEntityLogics(EntityLogicsRegister& logicsRegister) const {
}