#include "TestUtils/ConsoleAppTests.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

std::unique_ptr<Application> ConsoleAppTests::APP;

void ConsoleAppTests::SetUpTestCase() {
    VoidTestApplication* testApp = new VoidTestApplication(new DesktopPlatform(0, nullptr));
    testApp->retRes_createModuleFactory.reset(new ConsoleAppModuleFactory);

    ASSERT_TRUE(testApp->init());

    ASSERT_TRUE(GetEnv()->getETSystem());
    ASSERT_TRUE(GetEnv()->getPlatform());

    APP.reset(testApp);
}

void ConsoleAppTests::TearDownTestCase() {
    APP.reset();
}