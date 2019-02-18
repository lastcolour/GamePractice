#ifndef __ASSETS_TEST_HPP__
#define __ASSETS_TEST_HPP__

#include <gtest/gtest.h>

#include <memory>

class Application;

class AssetsTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<Application> APP;
};

#endif /* __ASSETS_TEST_HPP__ */ 