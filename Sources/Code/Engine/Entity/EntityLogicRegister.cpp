#include "Entity/EntityLogicRegister.hpp"

EntityLogicRegister::EntityLogicRegister() {
}

EntityLogicRegister::~EntityLogicRegister() {
}

void EntityLogicRegister::registerFromReflectCtx(ReflectContext& ctx) {
    auto classInfo = ctx.getRegisteredClassInfo();
    if(!classInfo) {
        return;
    }
    logics.push_back(classInfo);
}

std::vector<ClassInfo*>& EntityLogicRegister::getLogicClasses() {
    return logics;
}