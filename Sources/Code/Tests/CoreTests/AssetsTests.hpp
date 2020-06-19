#ifndef __ASSETS_TEST_HPP__
#define __ASSETS_TEST_HPP__

#include "TestUtils/VoidAppTests.hpp"

class DesktopAssets;

class AssetsTests : public VoidAppTests {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<DesktopAssets> ASSETS;
};

#endif /* __ASSETS_TEST_HPP__ */ 