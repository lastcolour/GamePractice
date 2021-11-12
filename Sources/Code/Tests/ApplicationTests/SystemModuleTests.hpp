#ifndef __SYSTEM_MODULE_TESTS_HPP__
#define __SYSTEM_MODULE_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class SystemModuleTests : public EngineTests {
protected:

    void SetUp() override;
    void TearDown() override;
};

#endif /* __SYSTEM_MODULE_TESTS_HPP__ */