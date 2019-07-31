#include "Platforms/Android/AndroidLogger.hpp"

#include <cassert>

#include <android/log.h>

namespace {
const char* LOG_TAG = "App";
}

AndroidLogger::AndroidLogger() :
    logLevel(LogLevel::Debug) {
}

AndroidLogger::~AndroidLogger() {
}

bool AndroidLogger::init() {
    ETNode<ETLogger>::connect(getEntityId());
    return true;
}

void AndroidLogger::deinit() {
    ETNode<ETLogger>::disconnect();
}

void AndroidLogger::ET_logMessage(LogLevel lvl, const std::string& msg) {
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

void AndroidLogger::ET_setLogLevel(LogLevel lvl) {
    logLevel = lvl;
}

std::string AndroidLogger::formatMessage(LogLevel lvl, const std::string& msg) {
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

void AndroidLogger::printMessasge(LogLevel lvl, const std::string& msg) {
    android_LogPriority logPriority = ANDROID_LOG_DEFAULT;
    switch(lvl) {
        case LogLevel::Debug: {
            logPriority = ANDROID_LOG_DEBUG;
            break;
        }
        case LogLevel::Info: {
            logPriority = ANDROID_LOG_INFO;
            break;
        }
        case LogLevel::Warning: {
            logPriority = ANDROID_LOG_WARN;
            break;
        }
        case LogLevel::Error: {
            logPriority = ANDROID_LOG_ERROR;
            break;
        }
        case LogLevel::Fatal:
            logPriority = ANDROID_LOG_FATAL;
            break;
        default: {
            assert(false && "Invalid log level");
            break;
        }
    }
    __android_log_print(logPriority, LOG_TAG, "%s", msg.c_str());
}
