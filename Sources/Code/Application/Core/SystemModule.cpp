#include "Core/SystemModule.hpp"
#include "ETApplicationInterfaces.hpp"

#include <cassert>
#include <chrono>

SystemModule::SystemModule(const std::string& moduleName) :
    name(moduleName) {
    assert(!name.empty() && "Empty module name");
}

bool SystemModule::init() {

    auto initStartT = std::chrono::high_resolution_clock::now();

    if(logicsContainer) {
        LogError("[SystemModule::init] Double init of module: '{0}'", name);
        return false;
    }
    logicsContainer = std::move(getSystemLogics());
    if(!logicsContainer) {
        LogError("[SystemModule::init] Can't get system logics for module: '{0}'", name);
        return false;
    }
    if (logicsContainer->empty()) {
        LogError("[SystemModule::init] Empty system logics for module: '{0}'", name);
        return false;
    }
    if(!logicsContainer->init()) {
        LogError("[SystemModule::init] Init fail of module: '{0}'", name);
        return false;
    }

    LogDebug("[DesktopAssets] Init module '{0}' in {1} ms", name,
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - initStartT).count());

    return true;
}