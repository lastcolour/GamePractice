#include "Core/GlobalEnvironment.hpp"
#include "Core/ETSystem.hpp"

#include <cassert>

GlobalEnvironment* ENV = nullptr;

GlobalEnvironment::GlobalEnvironment() {
    if(ENV) {
        assert(false && "Global ENV already exists");
    } else {
        ENV = this;
    }
    etSystem.reset(new ETSystem);
}

GlobalEnvironment::~GlobalEnvironment() {
    if(ENV == this) {
        ENV = nullptr;
    }
}

ETSystem* GlobalEnvironment::GetETSystem() {
    return etSystem.get();
}

GlobalEnvironment* GetEnv() {
    return ENV;
}