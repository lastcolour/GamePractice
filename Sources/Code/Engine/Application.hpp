#ifndef __APPLICATION_HPP__
#define __APPLICATION_HPP__

#include <vector>
#include <memory>

class SystemModule;

namespace Core {

class GlobalEnvironment;

} // namespace Core

class Application {
public:

    using ModuleListT = std::vector<std::unique_ptr<SystemModule>>;

public:

    Application();
    virtual ~Application();

    int run();

protected:

    bool init();
    void mainLoop();
    void deinit();

protected:

    virtual void buildModules(ModuleListT& modules);

private:

    Application(const Application& app) = delete;
    Application& operator=(const Application& app) = delete;

private:

    std::unique_ptr<Core::GlobalEnvironment> globalEnv;
    ModuleListT systemModules;
};

#endif /* __APPLICATION_HPP__ */