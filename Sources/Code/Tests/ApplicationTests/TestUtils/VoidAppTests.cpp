#include "TestUtils/VoidAppTests.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

std::unique_ptr<Application> VoidAppTests::APP;

void VoidAppTests::SetUpTestCase() {
    VoidTestApplication* testApp = new VoidTestApplication(new DesktopPlatform(0, nullptr));

    ASSERT_TRUE(testApp->init());

    ASSERT_TRUE(GetEnv()->getETSystem());

    APP.reset(testApp);
}

void VoidAppTests::TearDownTestCase() {
    APP.reset();
}