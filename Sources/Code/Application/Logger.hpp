#ifndef __LOGGER_HPP__
#define __LOGGER_HPP__

#include "Platforms/LogStream.hpp"
#include "Environment.hpp"
#include "Buffer.hpp"

#include <memory>
#include <vector>
#include <string>
#include <cstdio>

class LogStream;

enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Silent
};

class Logger {

    typedef std::unique_ptr<LogStream> LogStreamPtr;

public:

    Logger(std::unique_ptr<LogStream>&& logStream);
    virtual ~Logger() = default;

    virtual void message(LogLevel logLevel, const std::string& msg);
    virtual void setLogLevel(LogLevel logLvl);
    virtual void addStream(std::unique_ptr<LogStream>&& stream);

private:

    LogLevel logLevel { LogLevel::Debug };
    std::vector<LogStreamPtr> logStreams;
};

template<typename T>
T ConvertToPrintable(const T& val) {
    return val;
}

const char* ConvertToPrintable(const std::string& str);

template <typename... Args>
void StringFormatImpl(Buffer& buff, const std::string& format, Args&& ... args) {
    auto resStrSize = std::snprintf(static_cast<char*>(buff.getData()), buff.getSize(), format.c_str(), args...);
    if(resStrSize + 1 > buff.getSize()) {
        buff.resize(resStrSize + 1);
        StringFormatImpl(buff, format, std::forward<Args>(args)...);
    }
}

template<typename ... ArgsT>
std::string StringFormat(const std::string& format, const ArgsT& ... args)
{
    Buffer buff(format.size());
    StringFormatImpl(buff, format.c_str(), ConvertToPrintable(args)...);
    return buff.getString();
}

template<typename ... ArgsT>
void LogDebug(const char* msg, const ArgsT& ... args) {
    GetEnv()->getLogger()->message(LogLevel::Debug, StringFormat(msg, args ... ));
}

template<typename ... ArgsT>
void LogInfo(const char* msg, const ArgsT& ... args) {
    GetEnv()->getLogger()->message(LogLevel::Info, StringFormat(msg, args ... ));
}

template<typename ... ArgsT>
void LogWarning(const char* msg, const ArgsT& ... args) {
    GetEnv()->getLogger()->message(LogLevel::Warning, StringFormat(msg, args ... ));
}

template<typename ... ArgsT>
void LogError(const char* msg, const ArgsT& ... args) {
    GetEnv()->getLogger()->message(LogLevel::Error, StringFormat(msg, args ... ));
}

template<typename ... ArgsT>
void LogFatal(const char* msg, const ArgsT& ... args) {
    GetEnv()->getLogger()->message(LogLevel::Fatal, StringFormat(msg, args ... ));
}

#endif /* __LOGGER_HPP__ */