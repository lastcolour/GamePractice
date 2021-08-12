#include "Core/ETSystem.hpp"
#include "Core/GlobalEnvironment.hpp"

ET::ETSystem* GetETSystem() {
    auto env = GetEnv();
    if(!env) {
        return nullptr;
    }
    return env->GetETSystem();
}
namespace ET {

ETSystem::ETSystem() :
    entityIdGen() {
}

ETSystem::~ETSystem() {
}

} // namespace ET