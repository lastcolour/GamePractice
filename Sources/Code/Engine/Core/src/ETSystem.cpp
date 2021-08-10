#include "Core/ETSystem.hpp"
#include "Core/GlobalEnvironment.hpp"

ETSystem* GetETSystem() {
    auto env = GetEnv();
    if(!env) {
        return nullptr;
    }
    return env->GetETSystem();
}

ETSystem::ETSystem() :
    entityIdGen() {
}

ETSystem::~ETSystem() {
}