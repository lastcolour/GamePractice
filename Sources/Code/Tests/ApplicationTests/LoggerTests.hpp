#ifndef __LOGGER_TESTS_HPP__
#define __LOGGER_TESTS_HPP__

#include "TestUtils/ConsoleAppTests.hpp"

class TestLogger;

class LoggerTests : public ConsoleAppTests {
protected:

    void SetUp() override;
    void TearDown() override;

    const char* getLoggedMessage() const;

protected:

    std::unique_ptr<TestLogger> logger;
};

#endif /* __LOGGER_TESTS_HPP__ */