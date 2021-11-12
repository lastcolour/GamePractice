#ifndef __GLOBAL_ENVIRONMENT_HPP__
#define __GLOBAL_ENVIRONMENT_HPP__

class TasksRunner;

namespace Reflect {

class ClassInfoManager;

} // namespace Reflect

namespace Memory {

class MainMemoryAllocator;

} // namespace Memory

namespace ET {

class ETSystem;

} // namespace ET

namespace Core {

class GlobalData;

class GlobalEnvironment {
public:

    static GlobalEnvironment* ENV;

public:

    GlobalEnvironment();
    ~GlobalEnvironment();

    ET::ETSystem* GetETSystem() {
        return etSystem.get();
    }

    GlobalData* GetGlobalData() {
        return globalData.get();
    }

    TasksRunner* GetTasksRunner() {
        return taskRunner.get();
    }

    Memory::MainMemoryAllocator* GetMemoryAllocator() {
        return memoryAllocator.get();
    }

    Reflect::ClassInfoManager* GetClassInfoManager() {
        return classInfoManager.get();
    }

private:

    std::unique_ptr<Memory::MainMemoryAllocator> memoryAllocator;
    std::unique_ptr<GlobalData> globalData;
    std::unique_ptr<ET::ETSystem> etSystem;
    std::unique_ptr<TasksRunner> taskRunner;
    std::unique_ptr<Reflect::ClassInfoManager> classInfoManager;
};

} // namespace Core

inline Core::GlobalEnvironment* GetEnv() {
    return Core::GlobalEnvironment::ENV;
}

#endif /* __GLOBAL_ENVIRONMENT_HPP__ */