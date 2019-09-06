#include "TestUtils/ConsoleAppTests.hpp"

#include <Application.hpp>
#include <CoreModule.hpp>
#include <Platforms/Desktop/DesktopPlatform.hpp>
#include <Render/RenderModule.hpp>
#include <Entity/Entity.hpp>
#include <Core/ETPrimitives.hpp>
#include <Entity/EntityModule.hpp>
#include <UI/UIModule.hpp>
#include <ETApplicationInterfaces.hpp>
#include <Game/GameModule.hpp>

std::unique_ptr<Application> ConsoleAppTests::APP;

namespace {

const char* TEST_OBJECT_NAME = "TestObject";

class ConsoleAppTest : public Application {
public:

    ConsoleAppTest() : Application(nullptr) {}
    virtual ~ConsoleAppTest() = default;

    bool init() { return Application::init(); }

protected:

    void buildModules(ModuleListT& modules) override {
        modules.emplace_back(new CoreModule);
        DesktopPlatform platform(0, nullptr);
        modules.emplace_back(platform.createPlatformModule());
        modules.emplace_back(new RenderModule);
        modules.emplace_back(new EntityModule);
        modules.emplace_back(new UIModule);
        modules.emplace_back(new GameModule);
    }
};

} // namespace

void ConsoleAppTests::SetUpTestCase() {
    ConsoleAppTest* consoleApp = new ConsoleAppTest();
    if (!consoleApp->init()) {
        delete consoleApp;
        ASSERT_TRUE(false);
    }
    APP.reset(consoleApp);
    ET_SendEvent(&ETLogger::ET_setLogLevel, LogLevel::Info);
}

void ConsoleAppTests::TearDownTestCase() {
    APP.reset();
}

void ConsoleAppTests::TearDown() {
    tempObject.clear();
}

void ConsoleAppTests::addLogicToOject(std::unique_ptr<Entity>& obj, std::unique_ptr<EntityLogic>& logic) {
    obj->addLogic(std::move(logic));
}

std::unique_ptr<Entity> ConsoleAppTests::createVoidObject() {
    std::unique_ptr<Entity> objectPtr(new Entity(TEST_OBJECT_NAME, GetETSystem()->createNewEntityId()));
    return objectPtr;
}