#include "ApplicationTests.hpp"
#include "Application.hpp"
#include "Platform.hpp"
#include "Environment.hpp"
#include "Game/Game.hpp"

#include <thread>
#include <atomic>

namespace {

class TestPlatform : public Platform {
public:
    TestPlatform(bool initOK, bool shouldRunOk) : updated(false), initStatus(initOK), shouldRunStatus(shouldRunOk) {}
    virtual ~TestPlatform() {}
    virtual bool init() override { return initStatus; }
    virtual bool shouldRun() override { return shouldRunStatus.load(); }
    virtual void update() override { updated.store(true); }

    void setShouldRun(bool flag) { shouldRunStatus.store(flag); }
    bool isUpdated() const { return updated.load(); }

private:
    std::atomic<bool> updated;
    std::atomic<bool> shouldRunStatus;
    bool initStatus;
};

class TestGame : public Game {
public:

    TestGame(bool initFlag, bool shouldRunFlag) :
        updated(false),
        shouldRunStatus(shouldRunFlag),
        initStatus(initFlag) {}

    virtual ~TestGame() {}
    bool init() override { return initStatus; }
    bool shouldRun() override { return shouldRunStatus.load(); }
    void update() override { updated.store(true); }

    void setShouldRun(bool flag) { shouldRunStatus.store(flag); }
    int isUpdated() { updated.load(); }

private:

    std::atomic<bool> updated;
    std::atomic<bool> shouldRunStatus;
    bool initStatus;
};

class TestApp : public Application {
public:

    TestApp(Platform* platform) :
        Application(platform),
        testGame(nullptr),
        needCreateGame(true),
        initStatus(true),
        shouldRunStatus(true) {}

    virtual ~TestApp() {}

    void setGameFlags(bool needCreate, bool initFlag, bool shouldRunFlag) {
        needCreateGame = needCreate;
        initStatus = initFlag;
        shouldRunStatus = shouldRunFlag;
    }

    TestGame* getTestGame() {
        TestGame* gamePtr = nullptr;
        {
            std::unique_lock<std::mutex> lock(mutex);
            gamePtr = testGame;
        }
        return gamePtr;
    }

protected:

    std::unique_ptr<Game> createGame() override {
        std::lock_guard<std::mutex> lock(mutex);
        testGame = needCreateGame ? new TestGame(initStatus, shouldRunStatus) : nullptr;
        return std::unique_ptr<Game>(testGame);
    }

private:

    std::mutex mutex;
    TestGame* testGame;
    bool needCreateGame;
    bool initStatus;
    bool shouldRunStatus;
};

}

ApplicationTests::ApplicationTests() :
    appRes(-1),
    appFinished(false) {
}

ApplicationTests::~ApplicationTests() {
}

void ApplicationTests::SetUp() {
    appRes = -1;
    appFinished = false;
}

void ApplicationTests::TearDown() {
    appRes = -1;
    appFinished = false;
}

bool ApplicationTests::startAppThread(Application& app) {
    std::thread appThread([&](){
        std::unique_lock<std::mutex> lock(mutex);
        appRes = app.run();
        appFinished = true;
        cvVar.notify_one();
    });
    appThread.detach();
    return true;
}

bool ApplicationTests::isAppFinished() {
    std::unique_lock<std::mutex> lock(mutex);
    while(!appFinished) {
        cvVar.wait(lock);
    }
    return appRes == 0;
}


TEST_F(ApplicationTests, StartAppWithNullPlatform) {
    TestApp app(nullptr);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithInvalidPlatform) {
    bool initStatus = false;
    bool shouldRunStatus = false;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, DoNotUpdateMainLoopIfNoNeedToRun) {
    bool initStatus = true;
    bool shouldRunStatus = false;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));
    int res = app.run();
    ASSERT_TRUE(res == 0);
}

TEST_F(ApplicationTests, CheckUpdateWhenPlatformOk) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestPlatform* testPlatform = new TestPlatform(initStatus, shouldRunStatus);
    TestApp app(testPlatform);

    ASSERT_TRUE(startAppThread(app));

    while(!testPlatform->isUpdated());

    testPlatform->setShouldRun(false);

    ASSERT_TRUE(isAppFinished());
}

TEST_F(ApplicationTests, TestInvalidGame) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));

    bool needCreateGame = false;
    initStatus = true;
    shouldRunStatus = false;
    app.setGameFlags(needCreateGame, initStatus, shouldRunStatus);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, TestCantInitGame) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));

    bool needCreateGame = true;
    initStatus = false;
    shouldRunStatus = false;
    app.setGameFlags(needCreateGame, initStatus, shouldRunStatus);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, TestGameShouldntRun) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));

    bool needCreateStatus = true;
    initStatus = true;
    shouldRunStatus = false;
    app.setGameFlags(needCreateStatus, initStatus, shouldRunStatus);
    int res = app.run();

    ASSERT_TRUE(res == 0);
}

TEST_F(ApplicationTests, CheckUpdateWhenGameOk) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestApp app(new TestPlatform(initStatus, shouldRunStatus));

    bool needCreateStatus = true;
    initStatus = true;
    shouldRunStatus = true;
    app.setGameFlags(needCreateStatus, initStatus, shouldRunStatus);

    ASSERT_TRUE(startAppThread(app));

    TestGame* testGame = nullptr;
    while(!(testGame = app.getTestGame()));
    while(!testGame->isUpdated());

    testGame->setShouldRun(false);

    ASSERT_TRUE(isAppFinished());
}

TEST_F(ApplicationTests, CheckEnvironmentInited) {
    ASSERT_FALSE(GetEnv());

    bool initStatus = true;
    bool shouldRunStatus = true;
    std::unique_ptr<TestApp> app(new TestApp(new TestPlatform(initStatus, shouldRunStatus)));

    ASSERT_TRUE(GetEnv());
    ASSERT_EQ(&(GetEnv()->getApp()), &(*app));

    app.reset();

    ASSERT_FALSE(GetEnv());
}
