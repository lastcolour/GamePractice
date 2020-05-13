#include "TestUtils/SoundAppTests.hpp"
#include "Application.hpp"
#include "Entity/EntityModule.hpp"
#include "CoreModule.hpp"
#include "Audio/AudioModule.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Platforms/PlatformModule.hpp"

namespace {

std::unique_ptr<Application> APP;

const char* TEST_OBJECT_NAME = "TestObject";

class SoundAppTest : public Application {
public:

    SoundAppTest() : Application() {}
    virtual ~SoundAppTest() = default;

    bool init() { return Application::init(); }

protected:

    void buildModules(ModuleListT& modules) override {
        modules.emplace_back(new CoreModule);
        modules.emplace_back(new PlatformModule);
        modules.emplace_back(new EntityModule);
        modules.emplace_back(new AudioModule);
    }
};

} // namespace

void SoundAppTests::SetUpTestCase() {
    SoundAppTest* soundApp = new SoundAppTest();
    if (!soundApp->init()) {
        delete soundApp;
        ASSERT_TRUE(false);
    }
    APP.reset(soundApp);
    ET_SendEvent(&ETLogger::ET_setLogLevel, LogLevel::Info);
}

void SoundAppTests::TearDownTestCase() {
    APP.reset();
}