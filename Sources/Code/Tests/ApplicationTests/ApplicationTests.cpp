#include "ApplicationTests.hpp"
#include "TestUtils/FutureTimePoint.hpp"

#include <Platforms/Desktop/DesktopPlatform.hpp>
#include <Application.hpp>
#include <ETApplicationInterfaces.hpp>
#include <Core/SystemLogic.hpp>

#include <thread>
#include <atomic>

namespace {

const unsigned int APP_START_DELAY_MS = 5u * 1000u; // 5 sec
const unsigned int APP_FINISH_DELAY_MS = APP_START_DELAY_MS;

class TestSystemLogic : public SystemLogic {
public:

    virtual ~TestSystemLogic() = default;

    bool init() override {
        return false;
    }
    void deinit() {} 
};

class TestSystemModule : public SystemModule {
public:

    TestSystemModule() : SystemModule("TestModule") {}

protected:

    LogicsContainerPtrT getSystemLogics() const override {
        LogicsContainerPtrT container(
            new SystemLogicContainer<
                TestSystemLogic
            >
        );
        return container;
    }
};

class TestApplication : public Application {
public:

    TestApplication() : Application(nullptr) {}
    virtual ~TestApplication() = default;

protected:

    void buildModules(ModuleListT& modules) override {
        modules.emplace_back(new TestSystemModule);
    }
};

} // namespace

TEST_F(ApplicationTests, CheckRunWithInvalidModule) {
    TestApplication app;
    int retRes = app.run();
    ASSERT_EQ(retRes, 1);
}

TEST_F(ApplicationTests, CheckNormalFlow) {
    Application app(std::unique_ptr<Platform>(new DesktopPlatform(0, nullptr)));
    ASSERT_TRUE(GetETSystem());

    std::atomic<bool> isAppFinished(false);

    std::thread appThread([&app, &isAppFinished](){
        app.run();
        isAppFinished.store(true);
    });

    appThread.detach();

    FutureTimePoint futureTimeP(APP_START_DELAY_MS);
    while(!ET_IsExistNode<ETAppRunStateEvents>() && !futureTimeP.isHappened());

    ASSERT_FALSE(futureTimeP.isHappened());
    ASSERT_FALSE(isAppFinished.load());

    ET_SendEvent(&ETAppRunStateEvents::ET_onTerminate);

    futureTimeP.resetDelay(APP_FINISH_DELAY_MS);

    while(!isAppFinished.load() && !futureTimeP.isHappened());

    ASSERT_FALSE(futureTimeP.isHappened());
};