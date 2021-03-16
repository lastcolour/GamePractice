#ifndef __ET_LOGGER_HPP__
#define __ET_LOGGER_HPP__

#include "Core/StringFormat.hpp"
#include "Core/ETPrimitives.hpp"

enum class LogLevel {
    Debug = 0,
    Info,
    Warning,
    Error,
    Fatal,
    Silent
};

struct ETLogger {
    virtual ~ETLogger() = default;
    virtual void ET_logMessage(LogLevel lvl, const std::string& msg) = 0;
    virtual void ET_setLogLevel(LogLevel lvl) = 0;
};

template<typename ... ArgsT>
void LogDebug(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Debug, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogInfo(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Info, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogWarning(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Warning, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogError(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Error, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogFatal(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, LogLevel::Fatal, StringFormat(msg, args...));
}

#endif /* __ET_LOGGER_HPP__ */