#ifndef __VOID_TEST_APPLICATION_HPP__
#define __VOID_TEST_APPLICATION_HPP__

#include "Application.hpp"

#include "TestUtils/VoidTestModuleFactory.hpp"

class VoidTestApplication : public Application {
public:

    VoidTestApplication(Platform* platform) : Application(platform) {}
    virtual ~VoidTestApplication() = default;

    bool init() { return Application::init(); }

public:

    std::unique_ptr<ModuleFactory> retRes_createModuleFactory { new VoidTestModuleFactory };

protected:

    std::unique_ptr<ModuleFactory> createModuleFactory() override { return std::move(retRes_createModuleFactory); }
};

#endif /* __VOID_TEST_APPLICATION_HPP__ */