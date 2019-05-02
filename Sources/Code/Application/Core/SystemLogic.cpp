#include "Core/SystemLogic.hpp"
#include "SystemModule.hpp"

#include <cassert>

SystemLogic::SystemLogic() :
    module(nullptr) {
}

void SystemLogic::setParentModule(SystemModule& parentModule) {
    assert(!module && "Already have parent module");
    module = &parentModule;
}

EntityId SystemLogic::getEntityId() const {
    assert(module && "Invalid parent module");
    return module->getEntityId();
}