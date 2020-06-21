#include "Platform/PlatformLogger.hpp"

#include <iostream>
#include <cassert>

PlatformLogger::PlatformLogger() :
    logLevel(LogLevel::Debug) {
}

PlatformLogger::~PlatformLogger() {
    ETNode<ETLogger>::disconnect();
}

bool PlatformLogger::init() {
    ETNode<ETLogger>::connect(getEntityId());
    return true;
}

void PlatformLogger::deinit() {
    ETNode<ETLogger>::disconnect();
}

void PlatformLogger::ET_logMessage(LogLevel lvl, const std::string& msg) {
    if(logLevel > lvl) {
        return;
    }
    if(logLevel == LogLevel::Silent) {
        return;
    }
    if(msg.empty()) {
        return;
    }
    const std::string logMsg = formatMessage(lvl, msg);
    printMessasge(lvl, logMsg);
}

void PlatformLogger::ET_setLogLevel(LogLevel lvl) {
    logLevel = lvl;
}

std::string PlatformLogger::formatMessage(LogLevel lvl, const std::string& msg) {
    const char* prefix = nullptr;
    switch(lvl) {
        case LogLevel::Debug:
            prefix = "[Debug] ";
            break;
        case LogLevel::Info:
            prefix = "[Info] ";
            break;
        case LogLevel::Warning:
            prefix = "[Warning] ";
            break;
        case LogLevel::Error:
            prefix = "[Error] ";
            break;
        case LogLevel::Fatal:
            prefix = "[Fatal] ";
            break;
        case LogLevel::Silent:
        default:
            assert(false && "Invalid log level");
    }
    std::string logMsg = prefix;
    logMsg += msg;
    return logMsg;
}

void PlatformLogger::printMessasge(LogLevel lvl, const std::string& msg) {
    (void)lvl;
    std::cout << msg << std::endl;
}
