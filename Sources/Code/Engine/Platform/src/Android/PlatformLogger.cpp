#include "Platform/PlatformLogger.hpp"

#include <cassert>

#include <android/log.h>

namespace {

const char* LOG_TAG = "App";

} // namespace

PlatformLogger::PlatformLogger() :
    logLevel(Level::Debug) {
}

PlatformLogger::~PlatformLogger() {
}

bool PlatformLogger::init() {
    ETNode<ETLogger>::connect(getEntityId());
    return true;
}

void PlatformLogger::deinit() {
    ETNode<ETLogger>::disconnect();
}

void PlatformLogger::ET_logMessage(Level lvl, const std::string& msg) {
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
    android_LogPriority logPriority = ANDROID_LOG_DEFAULT;
    switch(lvl) {
        case Level::Debug: {
            logPriority = ANDROID_LOG_DEBUG;
            break;
        }
        case Level::Info: {
            logPriority = ANDROID_LOG_INFO;
            break;
        }
        case Level::Warning: {
            logPriority = ANDROID_LOG_WARN;
            break;
        }
        case Level::Error: {
            logPriority = ANDROID_LOG_ERROR;
            break;
        }
        case Level::Fatal:
            logPriority = ANDROID_LOG_FATAL;
            break;
        default: {
            assert(false && "Invalid log level");
            break;
        }
    }
    __android_log_print(logPriority, LOG_TAG, "%s", msg.c_str());
}
