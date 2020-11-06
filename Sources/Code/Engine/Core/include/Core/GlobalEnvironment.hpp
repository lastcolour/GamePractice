#ifndef __GLOBAL_ENVIRONMENT_HPP__
#define __GLOBAL_ENVIRONMENT_HPP__

#include <memory>

class ETSystem;

class GlobalEnvironment {
public:

    GlobalEnvironment();
    ~GlobalEnvironment();

    ETSystem* GetETSystem();

private:

    std::unique_ptr<ETSystem> etSystem;
};

GlobalEnvironment* GetEnv();

#endif /* __GLOBAL_ENVIRONMENT_HPP__ */