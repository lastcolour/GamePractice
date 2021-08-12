#include "Core/GlobalEnvironment.hpp"
#include "Core/ETSystem.hpp"
#include "Core/GlobalData.hpp"
#include "Parallel/TasksRunner.hpp"

#include <cassert>

GlobalEnvironment* ENV = nullptr;

GlobalEnvironment::GlobalEnvironment() {
    if(ENV) {
        assert(false && "Global ENV already exists");
    } else {
        ENV = this;
    }
    etSystem.reset(new ET::ETSystem);
    taskRunner.reset(new TasksRunner);
    globalData.reset(new GlobalData);
}

GlobalEnvironment::~GlobalEnvironment() {
    if(ENV == this) {
        ENV = nullptr;
    }
}

ET::ETSystem* GlobalEnvironment::GetETSystem() {
    return etSystem.get();
}

TasksRunner* GlobalEnvironment::GetTasksRunner() {
    return taskRunner.get();
}

GlobalData* GlobalEnvironment::GetGlobalData() {
    return globalData.get();
}

GlobalEnvironment* GetEnv() {
    return ENV;
}