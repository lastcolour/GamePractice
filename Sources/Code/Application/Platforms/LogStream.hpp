#ifndef __LOG_STREAM_HPP__
#define __LOG_STREAM_HPP__

#include <string>

enum class LogLevel;

class LogStream {
public:
    
    virtual ~LogStream() = default;

    virtual void logMessage(LogLevel logLvl, const std::string& msg) = 0;
};

#endif /* __LOG_STREAM_HPP__ */