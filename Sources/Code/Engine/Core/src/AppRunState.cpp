#include "Core/App/AppRunState.hpp"
#include "Core/GlobalEnvironment.hpp"
#include "Parallel/TasksRunner.hpp"

AppRunState::AppRunState() {
}

bool AppRunState::init() {
    ETNode<ETAppRunStateEvents>::connect(getEntityId());
    return true;
}

void AppRunState::deinit() {
    GetEnv()->GetTasksRunner()->stop();
    ETNode<ETAppRunStateEvents>::disconnect();
}

void AppRunState::ET_onTerminate() {
    GetEnv()->GetTasksRunner()->stop();
}

void AppRunState::ET_onPause() {
}

void AppRunState::ET_onResume() {
}