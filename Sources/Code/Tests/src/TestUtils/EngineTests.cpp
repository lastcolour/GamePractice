#include "TestUtils/EngineTests.hpp"

#include "Application.hpp"
#include "Render/RenderModule.hpp"
#include "Entity.hpp"
#include "Entity/EntityModule.hpp"
#include "UI/UIModule.hpp"
#include "Game/GameModule.hpp"
#include "Platform/PlatformModule.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Audio/AudioModule.hpp"
#include "Desktop/DesktopAssets.hpp"
#include "CoreModule.hpp"

void HACK_ASSERT_TRUE(bool value) {
    ASSERT_TRUE(value);
}

namespace {

std::unique_ptr<Application> APP;

const char* TEST_OBJECT_NAME = "TestObject";

class EngineTestsApp : public Application {
public:

    EngineTestsApp(ETestAppModules testAppModules) :
        Application(),
        appModules(testAppModules) {}

    virtual ~EngineTestsApp() = default;

    bool init() { return Application::init(); }

protected:

    void buildModules(ModuleListT& modules) override {
        if(appModules == ETestAppModules::Void) {
            return;
        } else if (appModules == ETestAppModules::CheckAssets) {
            modules.emplace_back(new PlatformModule);
        } else if(appModules == ETestAppModules::CheckEntities) {
            modules.emplace_back(new PlatformModule);
            modules.emplace_back(new EntityModule);
        } else if(appModules == ETestAppModules::CheckSound) {
            modules.emplace_back(new PlatformModule);
            modules.emplace_back(new EntityModule);
            modules.emplace_back(new AudioModule);
        } else if(appModules == ETestAppModules::CheckRender) {
            modules.emplace_back(new EntityModule);
            modules.emplace_back(new PlatformModule);
            modules.emplace_back(new RenderModule);
            // modules.emplace_back(new UIModule);
            // modules.emplace_back(new GameModule);
        }
    }

private:

    ETestAppModules appModules;
};

} // namespace


void EngineTests::CreateTestApp(ETestAppModules appModules) {
    EngineTestsApp* consoleApp = new EngineTestsApp(appModules);
    if (!consoleApp->init()) {
        delete consoleApp;
        ASSERT_TRUE(false) << "Can't initialize APP";
    }
    ASSERT_FALSE(APP.get()) << "APP Already Exists";

    APP.reset(consoleApp);
    ET_SendEvent(&ETLogger::ET_setLogLevel, ETLogger::Level::Info);
}

void EngineTests::TearDownTestCase() {
    APP.reset();
}

void EngineTests::TearDown() {
    for(auto entId : tempObject) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, entId);
    }
    tempObject.clear();

    size_t entitiesCount = 0;
    ET_SendEventReturn(entitiesCount, &ETEntityManager::ET_getEntitiesCount);

    EXPECT_EQ(entitiesCount, 0);
}

Entity* EngineTests::createVoidObject() {
    Entity* resEntity;
    ET_SendEventReturn(resEntity, &ETEntityManager::ET_createRawEntity, TEST_OBJECT_NAME);
    tempObject.push_back(resEntity->getEntityId());
    return resEntity;
}