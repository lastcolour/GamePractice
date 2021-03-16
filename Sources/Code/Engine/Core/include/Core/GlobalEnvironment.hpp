#ifndef __GLOBAL_ENVIRONMENT_HPP__
#define __GLOBAL_ENVIRONMENT_HPP__

class ETSystem;
class TasksRunner;
class GlobalData;

class GlobalEnvironment {
public:

    GlobalEnvironment();
    ~GlobalEnvironment();

    ETSystem* GetETSystem();
    TasksRunner* GetTasksRunner();
    GlobalData* GetGlobalData();

private:

    std::unique_ptr<ETSystem> etSystem;
    std::unique_ptr<TasksRunner> taskRunner;
    std::unique_ptr<GlobalData> globalData;
};

GlobalEnvironment* GetEnv();

#endif /* __GLOBAL_ENVIRONMENT_HPP__ */