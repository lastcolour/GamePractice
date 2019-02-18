#ifndef __VOID_TEST_LOGGER_HPP__
#define __VOID_TEST_LOGGER_HPP__

#include "Logger.hpp"

#include "TestUtils/VoidTestLogStream.hpp"

class VoidTestLogger : public Logger {
public:

    VoidTestLogger(std::unique_ptr<LogStream>&& logStream) : Logger(std::move(logStream)) {}
    virtual ~VoidTestLogger() = default;

    void message(LogLevel logLevel, const std::string& msg) override {}
    void setLogLevel(LogLevel logLvl) override {}
    void addStream(std::unique_ptr<LogStream>&& stream) override {}
};

#endif /* __VOID_TEST_LOGGER_HPP__ */