#ifndef __GAME_TESTS_HPP__
#define __GAME_TESTS_HPP__

#include <memory>

#include <gtest/gtest.h>

class VoidTestApplication;

class GameTests : public ::testing::Test {
protected:

    static void SetUpTestCase();
    static void TearDownTestCase();

protected:

    static std::unique_ptr<VoidTestApplication> app;

protected:

    void SetUp() override;
    void TearDown() override;
};

#endif /* __GAME_TESTS_HPP__ */