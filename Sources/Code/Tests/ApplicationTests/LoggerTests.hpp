#ifndef __LOGGER_TESTS_HPP__
#define __LOGGER_TESTS_HPP__

#include <gtest/gtest.h>

class LogStream;
class Logger;

class LoggerTests : public ::testing::Test {
protected:

    void SetUp() override;
    void TearDown() override;

    const char* getLoggedMessage() const;

protected:

    LogStream* stream;
    std::unique_ptr<Logger> logger;
};

#endif /* __LOGGER_TESTS_HPP__ */