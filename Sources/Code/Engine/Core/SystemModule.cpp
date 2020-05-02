#include "Core/SystemModule.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/EntityLogicRegister.hpp"

#include <cassert>
#include <chrono>

namespace {

const char* MODULE_CONFIGS_DIR = "Configs/Modules";

} // namespace

SystemModule::SystemModule(const char* moduleName) :
    name(moduleName) {
    assert(!name.empty() && "Empty module name");
}

SystemModule::~SystemModule() {
    LogDebug("[SystemModule::SystemModule] Deinit module: '%s'", name);
    if(logicsContainer) {
        logicsContainer->deinit();
    }
}

bool SystemModule::init() {
    auto initStartT = std::chrono::high_resolution_clock::now();

    configs = getSystemConfigs();
    if(configs) {
        JSONNode node;
        std::string configFile = StringFormat("%s/%s.json", MODULE_CONFIGS_DIR, name);
        ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, configFile.c_str());
        if(node) {
            configs->serialize(node);
        } else {
            LogError("[SystemModule::init] Can't load config for module: %s", name);
            configs.reset();
            return false;
        }
    }

    if(logicsContainer) {
        LogError("[SystemModule::init] Double init of module: '%s'", name);
        return false;
    }
    logicsContainer = std::move(getSystemLogics());
    if(!logicsContainer) {
        LogError("[SystemModule::init] Can't get system logics for module: '%s'", name);
        return false;
    }
    if(logicsContainer->empty()) {
        LogError("[SystemModule::init] Empty system logics for module: '%s'", name);
        return false;
    }

    EntityLogicRegister logicRegister;
    registerEntityLogics(logicRegister);

    if(!logicsContainer->init()) {
        LogError("[SystemModule::init] Init fail of module: '%s'", name);
        return false;
    }

    LogDebug("[SystemModule::init] Init module: '%s' in %d ms", name,
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - initStartT).count());

    return true;
}