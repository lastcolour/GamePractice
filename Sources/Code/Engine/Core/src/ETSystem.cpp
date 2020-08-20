#include "Core/ETSystem.hpp"

#include <cassert>

ETSystem* ET_SYSTEM = nullptr;

namespace ET {

int GLOBAL_ET_ID = 0;

int GetNextETId() {
    ++GLOBAL_ET_ID;
    return GLOBAL_ET_ID;
}

} // namespace ET

ETSystem* GetETSystem() {
    return ET_SYSTEM;
}

ETSystem::ETSystem() :
    entityIdGen() {
    if(!ET_SYSTEM) {
        ET_SYSTEM = this;
    } else {
        assert(false && "Already exist other ETSystem");
    }
}

ETSystem::~ETSystem() {
    if(this == ET_SYSTEM) {
        ET_SYSTEM = nullptr;
    }
}