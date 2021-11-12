#ifndef __ASSETS_TEST_HPP__
#define __ASSETS_TEST_HPP__

#include "TestUtils/EngineTests.hpp"

class DesktopAssets;

class AssetsTests : public EngineTests {
protected:

    static void SetUpTestCase() {
        CreateTestApp(ETestAppModules::CheckAssets);
    }
};

#endif /* __ASSETS_TEST_HPP__ */ 