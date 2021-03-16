#ifndef __CONSOLE_APP_TESTS_HPP__
#define __CONSOLE_APP_TESTS_HPP__

#include "Entity.hpp"

#include <gtest/gtest.h>

void HACK_ASSERT_TRUE(bool value);

class Application;

class ConsoleAppTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

    void TearDown() override;

protected:

    Entity* createVoidObject();

private:

    std::vector<EntityId> tempObject;
};

#endif /* __CONSOLE_APP_TESTS_HPP__ */

