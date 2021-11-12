#ifndef __ET_LOGGER_HPP__
#define __ET_LOGGER_HPP__

#include "Core/StringFormat.hpp"
#include "Core/ETSystem.hpp"

struct ETLogger {
public:

    enum class Level {
        Debug = 0,
        Info,
        Warning,
        Error,
        Fatal,
        Silent
    };

public:
    virtual ~ETLogger() = default;
    virtual void ET_logMessage(Level lvl, const std::string& msg) = 0;
    virtual void ET_setLogLevel(Level lvl) = 0;
};

template<typename ... ArgsT>
void LogDebug(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, ETLogger::Level::Debug, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogInfo(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, ETLogger::Level::Info, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogWarning(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, ETLogger::Level::Warning, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogError(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, ETLogger::Level::Error, StringFormat(msg, args...));
}

template<typename ... ArgsT>
void LogFatal(const char* msg, const ArgsT& ... args) {
    ET_SendEvent(&ETLogger::ET_logMessage, ETLogger::Level::Fatal, StringFormat(msg, args...));
}

#endif /* __ET_LOGGER_HPP__ */