#ifndef __GLOBAL_ENVIRONMENT_HPP__
#define __GLOBAL_ENVIRONMENT_HPP__

#include <memory>

class ETSystem;
class Profiler;

class GlobalEnvironment {
public:

    GlobalEnvironment();
    ~GlobalEnvironment();

    ETSystem* GetETSystem();
    Profiler* GetProfiler();

private:

    std::unique_ptr<ETSystem> etSystem;
    std::unique_ptr<Profiler> profiler;
};

GlobalEnvironment* GetEnv();

#endif /* __GLOBAL_ENVIRONMENT_HPP__ */