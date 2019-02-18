#include "Logger.hpp"

#include <cassert>

Logger::Logger(std::unique_ptr<LogStream>&& logStream) {
    addStream(std::move(logStream));
}

void Logger::addStream(std::unique_ptr<LogStream>&& stream) {
    if(!stream) {
        assert(false && "Invalid stream");
        return;
    }
    logStreams.push_back(std::move(stream));
}

void Logger::setLogLevel(LogLevel logLvl) {
    logLevel = logLvl;
}

void Logger::message(LogLevel logLvl, const std::string& msg) {
    if(logLevel > logLvl) {
        return;
    }
    if(logLevel == LogLevel::Silent) {
        return;
    }
    if(msg.empty()) {
        return;
    }
    for(auto& logStream : logStreams) {
        logStream->logMessage(logLvl, msg);
    }
}

const char* ConvertToPrintable(const std::string& str) {
    return str.c_str();
}