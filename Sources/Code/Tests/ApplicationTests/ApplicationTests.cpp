#include "ApplicationTests.hpp"
#include "Application.hpp"
#include "TestUtils/VoidTestPlatform.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

#include <thread>
#include <atomic>

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
    VoidTestApplication app(nullptr);
    int res = app.run();

    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, CheckEnvironmet) {
    ASSERT_FALSE(GetEnv());

    std::unique_ptr<VoidTestApplication> app(new VoidTestApplication(new VoidTestPlatform));

    ASSERT_TRUE(GetEnv());

    app.reset();

    ASSERT_FALSE(GetEnv());
}

TEST_F(ApplicationTests, StartAppWithPlatformInitFail) {
    VoidTestPlatform* testPlatform = new VoidTestPlatform;
    testPlatform->retRes_init = false;
    VoidTestApplication app(testPlatform);

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithLoggerCreateFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createLogger = nullptr;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithAssetsCreateFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createAssets = nullptr;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithAssetsInitFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createAssets->retRes_init = false;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithSurfaceInitFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createSurface->retRes_init = false;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithSurfaceShowFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createSurface->retRes_show = false;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithRenderCreateFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createRender = nullptr;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithRenderInitFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createRender->retRes_init = false;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithGameCreateFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createGame = nullptr;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithGameInitFail) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    factory->retRes_createGame->retRes_init = false;

    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, CheckSurfaceUpdate) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    VoidTestSurface* surface = factory->retRes_createSurface.get();

    ASSERT_TRUE(startAppThread(app));

    while(true) {
        std::lock_guard<std::mutex> lock(surface->mutex);
        if(surface->callCount_update) {
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(surface->mutex);
        surface->retRes_shouldRun = false;
    }

    ASSERT_TRUE(isAppFinished());
}

TEST_F(ApplicationTests, CheckGameUpdate) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    VoidTestGame* game = factory->retRes_createGame.get();

    ASSERT_TRUE(startAppThread(app));

    while(true) {
        std::lock_guard<std::mutex> lock(game->mutex);
        if(game->callCount_update) {
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(game->mutex);
        game->retRes_shouldRun = false;
    }

    ASSERT_TRUE(isAppFinished());
}

TEST_F(ApplicationTests, CheckRenderUpdate) {
    VoidTestApplication app(new VoidTestPlatform);
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    VoidTestGame* game = factory->retRes_createGame.get();
    VoidTestRender* render = factory->retRes_createRender.get();

    ASSERT_TRUE(startAppThread(app));

    while(true) {
        std::lock_guard<std::mutex> lock(render->mutex);
        if(render->callCount_update) {
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(game->mutex);
        game->retRes_shouldRun = false;
    }

    ASSERT_TRUE(isAppFinished());
}

TEST_F(ApplicationTests, CheckApplicationWithVoidGame) {
    auto factory = new OnlyVoidGameModuleFactory;
    VoidTestGame* game = factory->retRes_createGame.get();

    VoidTestApplication app(new DesktopPlatform(0, nullptr));
    app.retRes_createModuleFactory.reset(factory);

    ASSERT_TRUE(startAppThread(app));

    while(true) {
        std::lock_guard<std::mutex> lock(game->mutex);
        if(game->callCount_update) {
            break;
        }
    }
    {
        std::lock_guard<std::mutex> lock(game->mutex);
        game->retRes_shouldRun = false;
    }

    ASSERT_TRUE(isAppFinished());
}
