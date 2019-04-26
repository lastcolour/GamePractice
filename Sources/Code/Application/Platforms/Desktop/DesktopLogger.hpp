#ifndef __DESKTOP_LOGGER_HPP__
#define __DESKTOP_LOGGER_HPP__

#include "ETApplicationInterfaces.hpp"

class DesktopLogger: public SystemLogic,
    public ETNode<ETLogger> {
public:

    DesktopLogger();
    virtual ~DesktopLogger();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETLogger
    void ET_logMessage(LogLevel lvl, const std::string& msg) override;
    void ET_setLogLevel(LogLevel lvl) override;

protected:

    std::string formatMessage(LogLevel lvl, const std::string& msg);
    void printMessasge(LogLevel lvl, const std::string& msg);

private:

    LogLevel logLevel;
};

#endif /* __DESKTOP_LOGGER_HPP__ */