#ifndef __VOID_TEST_LOG_STREAM_HPP__
#define __VOID_TEST_LOG_STREAM_HPP__

#include "Platforms/LogStream.hpp"

class VoidTestLogStream : public LogStream {
public:

    VoidTestLogStream() = default;
    virtual ~VoidTestLogStream() = default;

    void logMessage(LogLevel logLvl, const std::string& msg) override {}
};

#endif /* __VOID_TEST_LOG_STREAM_HPP__ */