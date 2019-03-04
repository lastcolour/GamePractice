#include "SystemLogic.hpp"
#include "Environment.hpp"
#include "Core/ETSystem.hpp"

#include <cassert>

SystemLogic::SystemLogic() :
    entityId(InvalidEntityId) {
}

SystemLogic::~SystemLogic() {
}

bool SystemLogic::init() {
    if(auto et = GetEnv()->getETSystem()) {
        entityId = et->createNewEntityId();
    } else {
        assert(false && "No ET system for ID generation");
    }
    return onInit();
}

bool SystemLogic::shouldRun() {
    return onShouldRun();
}

void SystemLogic::update() {
    onUpdate();
}

EntityId SystemLogic::getEntityId() const {
    assert(entityId != InvalidEntityId && "Access to not inited SystemLogic");
    return entityId;
}