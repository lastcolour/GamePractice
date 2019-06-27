#ifndef __ASSETS_TEST_HPP__
#define __ASSETS_TEST_HPP__

#include "TestUtils/VoidAppTests.hpp"

class Assets;

class AssetsTests : public VoidAppTests {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<Assets> ASSETS;
};

#endif /* __ASSETS_TEST_HPP__ */ 