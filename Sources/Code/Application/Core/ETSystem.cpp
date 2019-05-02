#include "Core/ETSystem.hpp"

#include <cassert>

ETSystem* ET_SYSTEM = nullptr;

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

ETSystem* GetETSystem() {
    return ET_SYSTEM;
}
