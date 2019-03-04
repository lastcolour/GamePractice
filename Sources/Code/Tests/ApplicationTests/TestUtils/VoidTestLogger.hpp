#ifndef __VOID_TEST_LOGGER_HPP__
#define __VOID_TEST_LOGGER_HPP__

#include "Platform.hpp"

class VoidTestLogger : public Logger {
public:

    VoidTestLogger() = default;
    virtual ~VoidTestLogger() = default;

    // ETLogger
    void ET_logMessage(LogLevel lvl, const std::string& msg) override {}
    void ET_setLogLevel(LogLevel lvl) override {}

protected:

    // SystemLogic
    bool onInit() override { return true; }

protected:

    // Logger
    virtual void printMessasge(LogLevel logLevel, const std::string& msg) override {}
    virtual std::string formatMessage(LogLevel logLevel, const std::string& msg) override { return ""; }
};

#endif /* __VOID_TEST_LOGGER_HPP__ */