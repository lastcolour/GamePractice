#ifndef __VOID_TEST_APP_HPP__
#define __VOID_TEST_APP_HPP__

#include <gtest/gtest.h>
#include <memory>

class Application;

class VoidAppTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<Application> APP;
};

#endif /* __VOID_TEST_APP_HPP__ */