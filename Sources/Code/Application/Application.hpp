#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <memory>
#include <vector>

class SystemModule;
class Platform;
class ETSystem;

class Application {

    friend class Environment;

public:

    explicit Application(std::unique_ptr<Platform>&& platform);
    virtual ~Application();

    int run();

protected:

    bool init();
    void mainLoop();

private:

    Application() = delete;
    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;

private:

    std::unique_ptr<ETSystem> etSystem;
    std::vector<std::unique_ptr<SystemModule>> systemModules;
};

#endif /* __APPLICATION_HPP__ */