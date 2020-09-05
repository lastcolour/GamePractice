#ifndef __PLATFORM_LOGGER_HPP__
#define __PLATFORM_LOGGER_HPP__

#include "Core/ETLogger.hpp"
#include "Core/SystemLogic.hpp"

#include <mutex>

class PlatformLogger: public SystemLogic,
    public ETNode<ETLogger> {
public:

    PlatformLogger();
    virtual ~PlatformLogger();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETLogger
    void ET_logMessage(LogLevel lvl, const std::string& msg) override;
    void ET_setLogLevel(LogLevel lvl) override;

protected:

    virtual std::string formatMessage(LogLevel lvl, const std::string& msg);
    virtual void printMessasge(LogLevel lvl, const std::string& msg);

private:

    LogLevel logLevel;
    std::mutex mutex;
};

#endif /* __PLATFORM_LOGGER_HPP__ */