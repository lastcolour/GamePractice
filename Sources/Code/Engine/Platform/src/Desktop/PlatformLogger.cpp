#include "Platform/PlatformLogger.hpp"

#include <iostream>
#include <cassert>

PlatformLogger::PlatformLogger() :
    logLevel(Level::Debug) {
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

void PlatformLogger::ET_logMessage(Level lvl, const std::string& msg) {
    std::lock_guard<std::mutex> lock(mutex);
    if(logLevel > lvl) {
        return;
    }
    if(logLevel == Level::Silent) {
        return;
    }
    if(msg.empty()) {
        return;
    }
    const std::string logMsg = formatMessage(lvl, msg);
    printMessasge(lvl, logMsg);
}

void PlatformLogger::ET_setLogLevel(Level lvl) {
    std::lock_guard<std::mutex> lock(mutex);
    logLevel = lvl;
}

std::string PlatformLogger::formatMessage(Level lvl, const std::string& msg) {
    const char* prefix = nullptr;
    switch(lvl) {
        case Level::Debug:
            prefix = "[Debug] ";
            break;
        case Level::Info:
            prefix = "[Info] ";
            break;
        case Level::Warning:
            prefix = "[Warning] ";
            break;
        case Level::Error:
            prefix = "[Error] ";
            break;
        case Level::Fatal:
            prefix = "[Fatal] ";
            break;
        case Level::Silent:
        default:
            assert(false && "Invalid log level");
    }
    std::string logMsg = prefix;
    logMsg += msg;
    return logMsg;
}

void PlatformLogger::printMessasge(Level lvl, const std::string& msg) {
    (void)lvl;
    std::cout << msg << std::endl;
}
