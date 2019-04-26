#include "Core/ETSystem.hpp"

ETSystem* ET_SYSTEM = nullptr;

ETSystem* GetETSystem() {
    return ET_SYSTEM;
}

bool ETSystem::init() {
    if(ET_SYSTEM) {
        return false;
    }
    ET_SYSTEM = this;
    return true;
}

void ETSystem::deinit() {
    if(this == ET_SYSTEM) {
        ET_SYSTEM = nullptr;
    }
}
