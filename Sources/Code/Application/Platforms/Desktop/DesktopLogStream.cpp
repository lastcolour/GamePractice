#include "Platforms/Desktop/DesktopLogStream.hpp"
#include "Logger.hpp"

#include <iostream>

void DesktopLogStream::logMessage(LogLevel logLvl, const std::string& msg) {
    const char* prefix = nullptr;
    switch(logLvl) {
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
        default:
            prefix = "[NO_LVL] ";
    }
    std::cout << prefix << msg << std::endl;
}