#ifndef __GLOBAL_ENVIRONMENT_HPP__
#define __GLOBAL_ENVIRONMENT_HPP__

#include <memory>

class ETSystem;
class TasksRunner;

class GlobalEnvironment {
public:

    GlobalEnvironment();
    ~GlobalEnvironment();

    ETSystem* GetETSystem();
    TasksRunner* GetTasksRunner();

private:

    std::unique_ptr<ETSystem> etSystem;
    std::unique_ptr<TasksRunner> taskRunner;
};

GlobalEnvironment* GetEnv();

#endif /* __GLOBAL_ENVIRONMENT_HPP__ */