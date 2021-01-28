#include "Core/GlobalEnvironment.hpp"
#include "Core/ETSystem.hpp"
#include "Parallel/TasksRunner.hpp"

#include <cassert>

GlobalEnvironment* ENV = nullptr;

GlobalEnvironment::GlobalEnvironment() {
    if(ENV) {
        assert(false && "Global ENV already exists");
    } else {
        ENV = this;
    }
    etSystem.reset(new ETSystem);
    taskRunner.reset(new TasksRunner);
}

GlobalEnvironment::~GlobalEnvironment() {
    if(ENV == this) {
        ENV = nullptr;
    }
}

ETSystem* GlobalEnvironment::GetETSystem() {
    return etSystem.get();
}

TasksRunner* GlobalEnvironment::GetTasksRunner() {
    return taskRunner.get();
}

GlobalEnvironment* GetEnv() {
    return ENV;
}