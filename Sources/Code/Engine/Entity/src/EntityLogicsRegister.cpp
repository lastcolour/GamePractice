#include "Entity/EntityLogicsRegister.hpp"

#include <cassert>

EntityLogicsRegister::EntityLogicsRegister(const char* module) :
    moduleName(module) {

    assert((module && module[0]) && "Empty module name");
}

EntityLogicsRegister::~EntityLogicsRegister() {
}

void EntityLogicsRegister::registerFromReflectCtx(ReflectContext& ctx) {
    auto classInfo = ctx.getRegisteredClassInfo();
    if(!classInfo) {
        return;
    }
    logics.push_back(classInfo);
}

std::vector<ClassInfo*>& EntityLogicsRegister::getLogicClasses() {
    return logics;
}

const char* EntityLogicsRegister::getModuleName() const {
    return moduleName;
}