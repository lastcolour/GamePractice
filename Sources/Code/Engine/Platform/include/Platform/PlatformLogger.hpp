#ifndef __PLATFORM_LOGGER_HPP__
#define __PLATFORM_LOGGER_HPP__

#include "Core/SystemLogic.hpp"

class PlatformLogger: public SystemLogic,
    public ETNode<ETLogger> {
public:

    PlatformLogger();
    virtual ~PlatformLogger();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETLogger
    void ET_logMessage(Level lvl, const std::string& msg) override;
    void ET_setLogLevel(Level lvl) override;

protected:

    virtual std::string formatMessage(Level lvl, const std::string& msg);
    virtual void printMessasge(Level lvl, const std::string& msg);

private:

    Level logLevel;
    std::mutex mutex;
};

#endif /* __PLATFORM_LOGGER_HPP__ */