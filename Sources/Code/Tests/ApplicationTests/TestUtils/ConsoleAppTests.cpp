#include "TestUtils/ConsoleAppTests.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"
#include "Game/GameObject.hpp"

std::unique_ptr<Application> ConsoleAppTests::APP;

namespace {

const char* TEST_OBJECT_NAME = "TestObj";

class ConsoleApplication : public Application {
public:
    ConsoleApplication() : Application(new DesktopPlatform(0, nullptr)) {}
    virtual ~ConsoleApplication() = default;

    bool init() { return Application::init(); }

protected:

    bool initSurface(Surface& surface) override {
        return surface.init();
    }

    bool initGame(Game& game) override {
        game.setRootObject("");
        return game.init();
    }
};

} // namespcae

void ConsoleAppTests::SetUpTestCase() {
    ConsoleApplication* consoleApp = new ConsoleApplication;

    ASSERT_TRUE(consoleApp->init());

    ASSERT_TRUE(GetEnv()->getETSystem());
    ASSERT_TRUE(GetEnv()->getPlatform());

    ET_SendEvent(&ETLogger::ET_setLogLevel, LogLevel::Warning);

    APP.reset(consoleApp);
}

void ConsoleAppTests::TearDownTestCase() {
    APP.reset();
}

std::unique_ptr<GameObject> ConsoleAppTests::createVoidObject() const {
    std::unique_ptr<GameObject> objectPtr(new GameObject(TEST_OBJECT_NAME, GetEnv()->getETSystem()->createNewEntityId()));
    return objectPtr;
}