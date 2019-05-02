#include "Core/SystemModule.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>
#include <chrono>

SystemModule::SystemModule(const std::string& moduleName) :
    name(moduleName),
    moduleId(GetETSystem()->createNewEntityId()) {
    assert(!name.empty() && "Empty module name");
}

bool SystemModule::init() {
    LogDebug("[SystemModule::init] Start init module: '%s'", name);

    auto initStartT = std::chrono::high_resolution_clock::now();

    if(logicsContainer) {
        LogError("[SystemModule::init] Double init of module: '%s'", name);
        return false;
    }
    logicsContainer = std::move(getSystemLogics());
    if(!logicsContainer) {
        LogError("[SystemModule::init] Can't get system logics for module: '%s'", name);
        return false;
    }
    if (logicsContainer->empty()) {
        LogError("[SystemModule::init] Empty system logics for module: '%s'", name);
        return false;
    }

    logicsContainer->setParentModule(*this);

    if(!logicsContainer->init()) {
        LogError("[SystemModule::init] Init fail of module: '%s'", name);
        return false;
    }

    LogDebug("[SystemModule::init] Finish init module: '%s' in %d ms", name,
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - initStartT).count());

    return true;
}

EntityId SystemModule::getEntityId() const {
    return moduleId;
}