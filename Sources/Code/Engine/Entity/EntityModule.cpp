#include "Entity/EntityModule.hpp"
#include "Entity/EntityManager.hpp"

EntityModule::EntityModule() :
    SystemModule("Entity") {}

EntityModule::LogicsContainerPtrT EntityModule::getSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            EntityManager>()
        );
    return container;
}

EntityModule::ConfigsPtrT EntityModule::getSystemConfigs() const {
    return nullptr;
}

void EntityModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}