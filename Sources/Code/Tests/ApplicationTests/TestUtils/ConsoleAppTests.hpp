#ifndef __CONSOLE_APP_TESTS_HPP__
#define __CONSOLE_APP_TESTS_HPP__

#include <gtest/gtest.h>
#include <memory>

class Application;
class GameObject;

class ConsoleAppTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<Application> APP;

protected:

    std::unique_ptr<GameObject> createVoidObject() const;
};

#endif /* __CONSOLE_APP_TESTS_HPP__ */