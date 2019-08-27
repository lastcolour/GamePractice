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