#include "TestUtils/VoidAppTests.hpp"

#include <Application.hpp>
#include <CoreModule.hpp>
#include <Platform.hpp>

std::unique_ptr<Application> VoidAppTests::APP;

namespace {

class VoidTestApp : public Application {
public:

    VoidTestApp() : Application(nullptr) {}
    virtual ~VoidTestApp() = default;

protected:

    void buildModules(ModuleListT& modules) override {
        modules.emplace_back(new CoreModule);
    }
};

} // namespace

void VoidAppTests::SetUpTestCase() {
    APP.reset(new VoidTestApp());
}

void VoidAppTests::TearDownTestCase() {
    APP.reset();
}