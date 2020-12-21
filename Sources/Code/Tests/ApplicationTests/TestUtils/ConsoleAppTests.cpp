#include "TestUtils/ConsoleAppTests.hpp"

#include "Application.hpp"
#include "Render/RenderModule.hpp"
#include "Entity.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/EntityModule.hpp"
#include "UI/UIModule.hpp"
#include "Game/GameModule.hpp"
#include "Platform/PlatformModule.hpp"
#include "Core/ETLogger.hpp"
#include "Entity/ETEntityManger.hpp"

namespace {

std::unique_ptr<Application> APP;

const char* TEST_OBJECT_NAME = "TestObject";

class ConsoleAppTest : public Application {
public:

    ConsoleAppTest() : Application() {}
    virtual ~ConsoleAppTest() = default;

    bool init() { return Application::init(); }

protected:

    void buildModules(ModuleListT& modules) override {
        modules.emplace_back(new EntityModule);
        modules.emplace_back(new PlatformModule);
        modules.emplace_back(new RenderModule);
        modules.emplace_back(new UIModule);
        modules.emplace_back(new GameModule);
    }
};

} // namespace

void HACK_ASSERT_TRUE(bool value) {
    ASSERT_TRUE(value);
}

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
    for(auto entId : tempObject) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, entId);
    }
    tempObject.clear();
}

Entity* ConsoleAppTests::createVoidObject() {
    Entity* resEntity;
    ET_SendEventReturn(resEntity, &ETEntityManager::ET_createRawEntity, TEST_OBJECT_NAME);
    tempObject.push_back(resEntity->getEntityId());
    return resEntity;
}