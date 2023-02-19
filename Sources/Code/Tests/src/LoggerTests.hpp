#ifndef __LOGGER_TESTS_HPP__
#define __LOGGER_TESTS_HPP__

#include "TestUtils/EngineTests.hpp"

class TestLogger;

class LoggerTests : public EngineTests {
protected:

    void SetUp() override;
    void TearDown() override;

    const char* getLoggedMessage() const;

protected:

    std::unique_ptr<TestLogger> logger;
};

#endif /* __LOGGER_TESTS_HPP__ */