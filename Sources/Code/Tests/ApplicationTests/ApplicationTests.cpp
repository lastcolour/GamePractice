#include "ApplicationTests.hpp"
#include "Application.hpp"
#include "Platform.hpp"
#include "Game/Game.hpp"

#include <thread>
#include <mutex>
#include <atomic>
#include <condition_variable>

namespace {

class TestPlatform : public Platform {
public:
    TestPlatform(bool initOK, bool shouldRunOk) : updateCount(0), initStatus(initOK), shouldRunStatus(shouldRunOk) {}
    virtual ~TestPlatform() {}
    virtual bool init() override { return initStatus; }
    virtual bool shouldRun() override { return shouldRunStatus.load(); }
    virtual void update() override { updateCount.fetch_add(1); }

    void setShouldRun(bool flag) { shouldRunStatus.store(flag); }
    int getUpdateCount() const { return updateCount.load(); }

private:
    std::atomic<int> updateCount;
    std::atomic<bool> shouldRunStatus;
    bool initStatus;
};

class TestGame : public Game {
public:

    TestGame(bool initFlag, bool shouldRunFlag) :
        updateCount(0),
        shouldRunStatus(shouldRunFlag),
        initStatus(initFlag) {}

    virtual ~TestGame() {}
    bool init() override { return initStatus; }
    bool shouldRun() override { return shouldRunStatus.load(); }
    void update() override { updateCount.fetch_add(1); }

    void setShouldRun(bool flag) { shouldRunStatus.store(flag); }
    int getUpdateCount() { updateCount.load(); }

private:

    std::atomic<int> updateCount;
    std::atomic<bool> shouldRunStatus;
    bool initStatus;
};

class TestApp : public Application {
public:

    TestApp(Platform* platform) :
        Application(platform),
        needCreateGame(true),
        initStatus(true),
        shouldRunStatus(true) {}

    virtual ~TestApp() {}

    void setGameFlags(bool needCreate, bool initFlag, bool shouldRunFlag) {
        needCreateGame = needCreate;
        initStatus = initFlag;
        shouldRunStatus = shouldRunFlag;
    }

    TestGame* getTestGame() { return testGame; }

protected:

    std::unique_ptr<Game> createGame() override {
        testGame = needCreateGame ? new TestGame(initStatus, shouldRunStatus) : nullptr;
        return std::unique_ptr<Game>(testGame);
    }

private:

    TestGame* testGame;
    bool needCreateGame;
    bool initStatus;
    bool shouldRunStatus;
};

}

TEST_F(ApplicationTests, StartAppWithNullPlatform) {
    Application app(nullptr);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithInvalidPlatform) {
    bool initStatus = false;
    bool shouldRunStatus = false;
    Application app(new TestPlatform(initStatus, shouldRunStatus));
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, DoNotUpdateMainLoopIfNoNeedToRun) {
    bool initStatus = true;
    bool shouldRunStatus = false;
    Application app(new TestPlatform(initStatus, shouldRunStatus));
    int res = app.run();
    ASSERT_TRUE(res == 0);
}

TEST_F(ApplicationTests, CheckUpdateWhenPlatformOk) {
    bool initStatus = true;
    bool shouldRunStatus = true;
    TestPlatform* testPlatform = new TestPlatform(initStatus, shouldRunStatus);
    Application app(testPlatform);

    ASSERT_EQ(testPlatform->getUpdateCount(), 0);

    int appRes = -1;
    bool appFinished = false;
    std::condition_variable cvVar;
    std::mutex mutex;
    std::thread appThread([&](){
        std::unique_lock<std::mutex> lock(mutex);
        appRes = app.run();
        appFinished = true;
        cvVar.notify_one();
    });
    appThread.detach();

    bool setShouldRun = false;
    while(!setShouldRun && testPlatform->getUpdateCount()) {
        testPlatform->setShouldRun(false);
        setShouldRun = true;
    }
    std::unique_lock<std::mutex> lock(mutex);
    while(!appFinished) {
        cvVar.wait(lock);
    }
    ASSERT_TRUE(appRes == 0);
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

    int appRes = -1;
    bool appFinished = false;
    std::condition_variable cvVar;
    std::mutex mutex;
    std::thread appThread([&](){
        std::unique_lock<std::mutex> lock(mutex);
        appRes = app.run();
        appFinished = true;
        cvVar.notify_one();
    });
    appThread.detach();

    auto testGame = app.getTestGame();
    ASSERT_TRUE(testGame != nullptr);

    bool setShouldRun = false;
    while(!setShouldRun && testGame->getUpdateCount()) {
        testGame->setShouldRun(false);
        setShouldRun = true;
    }
    std::unique_lock<std::mutex> lock(mutex);
    while(!appFinished) {
        cvVar.wait(lock);
    }
    ASSERT_TRUE(appRes == 0);
}
