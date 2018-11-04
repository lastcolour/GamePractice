#include "ApplicationTests.hpp"
#include "Application.hpp"
#include "Platform.hpp"

namespace {

class InvalidAppPlatform : public Platform {
public:
    virtual ~InvalidAppPlatform() {}
    virtual bool init() override { return false; }
};

}

TEST_F(ApplicationTests, StartAppWithNullPlatform) {
    Application app(nullptr);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}

TEST_F(ApplicationTests, StartAppWithInvalidPlatform) {
    Application app(new InvalidAppPlatform);
    int res = app.run();
    ASSERT_FALSE(res == 0);
}