#ifndef __SYSTEM_MODULE_TESTS_HPP__
#define __SYSTEM_MODULE_TESTS_HPP__

#include "TestUtils/VoidAppTests.hpp"

class SystemModuleTests : public VoidAppTests {
protected:

    void SetUp() override;
    void TearDown() override;
};

#endif /* __SYSTEM_MODULE_TESTS_HPP__ */