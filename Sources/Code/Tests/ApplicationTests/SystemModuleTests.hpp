#ifndef __SYSTEM_MODULE_TESTS_HPP__
#define __SYSTEM_MODULE_TESTS_HPP__

#include <gtest/gtest.h>

class SystemModuleTests : public ::testing::Test {
protected:

    void SetUp() override;
    void TearDown() override;
};

#endif /* __SYSTEM_MODULE_TESTS_HPP__ */