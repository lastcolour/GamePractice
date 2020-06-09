
#include "Core/SystemModule.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Entity/EntityLogicRegister.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <cassert>
#include <chrono>

namespace {

const char* MODULE_CONFIGS_DIR = "Modules";

} // namespace

SystemModule::SystemModule(const char* moduleName) :
    name(moduleName),
    isInitialized(false) {
    assert(!name.empty() && "Empty module name");
}

SystemModule::~SystemModule() {
    if(!isInitialized) {
        return;
    }
    LogDebug("[SystemModule::~SystemModule] Deinit module: '%s'", name);
    if(logicsContainer) {
        logicsContainer->deinit();
    }
}

JSONNode SystemModule::loadModuleConfigs() {
    JSONNode node;
    if(!ET_IsExistNode<ETAssets>()) {
        LogError("[SystemModule::loadModuleConfigs] Can't load config for a module '%s' because ETAssets node does not exist",
            name);
        return node;
    }
    std::string configFile = StringFormat("%s/%s.json", MODULE_CONFIGS_DIR, name);
    ET_SendEventReturn(node, &ETAssets::ET_loadJSONAsset, configFile.c_str());
    return node;
}

bool SystemModule::serializeConfigs() {
    ReflectContext ctx;
    reflectSystemConfigs(ctx);
    auto configClass = ctx.getRegisteredClassInfo();
    if(!configClass) {
        return true;
    }
    auto node = loadModuleConfigs();
    if(!node) {
        LogError("[SystemModule::serializeConfigs] Can't init module without configs: %s", name);
        return false;
    }

    if(!node.hasKey(configClass->getName())) {
        LogError("[SystemModule::serializeConfigs] Can't get required field from config file: '%s'", configClass->getName());
        return false;
    }
    auto classNode = node.object(configClass->getName());
    if(!classNode) {
        LogError("[SystemModule::serializeConfigs] Config field has non-object type: '%s'", configClass->getName());
        return false;
    }
    auto configInstance = configClass->createInstance();
    if(!configInstance.get()) {
        LogError("[SystemModule::serializeConfigs] Can't create instance of module configs '%s' for module: '%s'",
            configClass->getName(), name);
        return false;
    }
    if(!configInstance.readAllValuesFrom(classNode)) {
        LogError("[SystemModule::serializeConfigs] Can't read values of instance of module configs '%s' for module: '%s'",
            configClass->getName(), name);
        return false;
    }
    ET_SendEvent(&ETModuleConfigManager::ET_registerConfig, configInstance);
    return true;
}

bool SystemModule::init() {
    assert(!isInitialized && "Dobule module initializetion");

    auto initStartT = std::chrono::high_resolution_clock::now();

    if(!serializeConfigs()) {
        return false;
    }

    if(logicsContainer) {
        LogError("[SystemModule::init] Double init of module: '%s'", name);
        return false;
    }
    logicsContainer = std::move(createSystemLogics());
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

    if(!logicRegister.getLogicClasses().empty()) {
        if(!ET_IsExistNode<ETEntityManager>()) {
            LogError("[SystemModule::init] Module tries to regiser entity logics before EntityManager created: '%s'", name);
            return false;
        }
        bool logicRegisterRes = false;
        ET_SendEventReturn(logicRegisterRes, &ETEntityManager::ET_registerLogics, logicRegister);
        if(!logicRegisterRes) {
            LogError("[SystemModule::init] Can't register entity logics for module: '%s'", name);
            return false;
        }
    }

    isInitialized = true;
    if(!logicsContainer->init()) {
        LogError("[SystemModule::init] Init fail of module: '%s'", name);
        return false;
    }

    LogDebug("[SystemModule::init] Init module: '%s' in %d ms", name,
        std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::high_resolution_clock::now() - initStartT).count());

    return true;
}