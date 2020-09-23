#include "Core/ETSystem.hpp"
#include "Core/GlobalEnvironment.hpp"

namespace ET {

int GLOBAL_ET_ID = 0;

int GetNextETId() {
    ++GLOBAL_ET_ID;
    return GLOBAL_ET_ID;
}

} // namespace ET

ETSystem* GetETSystem() {
    return GetEnv()->GetETSystem();
}

ETSystem::ETSystem() :
    entityIdGen() {
}

ETSystem::~ETSystem() {
}