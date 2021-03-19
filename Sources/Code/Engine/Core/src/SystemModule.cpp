
#include "Core/SystemModule.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETApplication.hpp"
#include "Entity/EntityLogicsRegister.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Core/TimePoint.hpp"

#include <cassert>

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

bool SystemModule::init() {
    assert(!isInitialized && "Dobule module initializetion");

    auto initStarT = TimePoint::GetNowTime();

    createSystemConfigs();

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

    EntityLogicsRegister logicsRegister(name.c_str());
    registerEntityLogics(logicsRegister);

    if(!logicsRegister.getLogicClasses().empty()) {
        if(!ET_IsExistNode<ETEntityManager>()) {
            LogError("[SystemModule::init] Module tries to regiser entity logics before EntityManager created: '%s'", name);
            return false;
        }
        bool logicsRegisterRes = false;
        ET_SendEventReturn(logicsRegisterRes, &ETEntityManager::ET_registerLogics, logicsRegister);
        if(!logicsRegisterRes) {
            LogError("[SystemModule::init] Can't register entity logics for module: '%s'", name);
            return false;
        }
    }

    isInitialized = true;
    if(!logicsContainer->init()) {
        LogError("[SystemModule::init] Init fail of module: '%s'", name);
        return false;
    }

    int msValue = -static_cast<int>(initStarT.getMiliSecElapsedFrom(TimePoint::GetNowTime()));
    LogDebug("[SystemModule::init] Init module: '%s' in %d ms", name, msValue);

    return true;
}