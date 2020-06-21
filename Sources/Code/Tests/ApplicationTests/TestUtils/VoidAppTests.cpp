#include "TestUtils/VoidAppTests.hpp"

#include <Application.hpp>

std::unique_ptr<Application> VoidAppTests::APP;

namespace {

class VoidTestApp : public Application {
public:

    VoidTestApp() : Application() {}
    virtual ~VoidTestApp() = default;

    bool init() { return Application::init(); }

protected:

    void buildModules(ModuleListT& modules) override {}
};

} // namespace

void VoidAppTests::SetUpTestCase() {
    VoidTestApp* voidApp = new VoidTestApp();
    if (!voidApp->init()) {
        delete voidApp;
        ASSERT_TRUE(false);
    }
    APP.reset(voidApp);
}

void VoidAppTests::TearDownTestCase() {
    APP.reset();
}