#ifndef __DESKTOP_LOGGER_HPP__
#define __DESKTOP_LOGGER_HPP__

#include "Platform.hpp"

class DesktopLogger: public Logger {
public:

    DesktopLogger();
    virtual ~DesktopLogger();

    // ETLogger
    void ET_logMessage(LogLevel lvl, const std::string& msg) override;
    void ET_setLogLevel(LogLevel lvl) override;

protected:

    // SystemLogic
    bool onInit() override { return true; }

protected:

    // Logger
    std::string formatMessage(LogLevel lvl, const std::string& msg) override;
    void printMessasge(LogLevel lvl, const std::string& msg) override;

private:

    LogLevel logLevel;
};

#endif /* __DESKTOP_LOGGER_HPP__ */