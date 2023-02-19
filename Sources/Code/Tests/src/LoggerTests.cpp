#include "LoggerTests.hpp"
#include "Platform/PlatformLogger.hpp"

class TestLogger : public PlatformLogger {
public:
    virtual ~TestLogger() = default;

    const char* getLoggedMessage() const {
        return loggedMsg.c_str();
    }

protected:

    void printMessasge(ETLogger::Level level, const std::string& msg) override {
        loggedMsg += msg;
    }

    std::string formatMessage(ETLogger::Level level, const std::string& msg) override {
        return msg;
    }

private:

    std::string loggedMsg;
};

void LoggerTests::SetUp() {
    logger.reset(new TestLogger);
    logger->init();
}

void LoggerTests::TearDown() {
    logger.reset();
}

const char* LoggerTests::getLoggedMessage() const {
    return logger->getLoggedMessage();
}

TEST_F(LoggerTests, CheckLogEmptyMessage) {
    LogDebug("");
    ASSERT_STREQ(getLoggedMessage(), "");
}

TEST_F(LoggerTests, CheckLogDebug) {
    logger->ET_setLogLevel(ETLogger::Level::Debug);
    logger->ET_logMessage(ETLogger::Level::Debug, "D");
    logger->ET_logMessage(ETLogger::Level::Info, "I");
    logger->ET_logMessage(ETLogger::Level::Warning, "W");
    logger->ET_logMessage(ETLogger::Level::Error, "E");
    logger->ET_logMessage(ETLogger::Level::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}

TEST_F(LoggerTests, CheckLogInfo) {
    logger->ET_setLogLevel(ETLogger::Level::Info);
    logger->ET_logMessage(ETLogger::Level::Debug, "D");
    logger->ET_logMessage(ETLogger::Level::Info, "I");
    logger->ET_logMessage(ETLogger::Level::Warning, "W");
    logger->ET_logMessage(ETLogger::Level::Error, "E");
    logger->ET_logMessage(ETLogger::Level::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "IWEF");
}

TEST_F(LoggerTests, CheckLogWarning) {
    logger->ET_setLogLevel(ETLogger::Level::Warning);
    logger->ET_logMessage(ETLogger::Level::Debug, "D");
    logger->ET_logMessage(ETLogger::Level::Info, "I");
    logger->ET_logMessage(ETLogger::Level::Warning, "W");
    logger->ET_logMessage(ETLogger::Level::Error, "E");
    logger->ET_logMessage(ETLogger::Level::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "WEF");
}

TEST_F(LoggerTests, CheckLogError) {
    logger->ET_setLogLevel(ETLogger::Level::Error);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "EF");
}

TEST_F(LoggerTests, CheckLogFatal) {
    logger->ET_setLogLevel(ETLogger::Level::Fatal);
    logger->ET_logMessage(ETLogger::Level::Debug, "D");
    logger->ET_logMessage(ETLogger::Level::Info, "I");
    logger->ET_logMessage(ETLogger::Level::Warning, "W");
    logger->ET_logMessage(ETLogger::Level::Error, "E");
    logger->ET_logMessage(ETLogger::Level::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "F");
}

TEST_F(LoggerTests, CheckLogSilent) {
    logger->ET_setLogLevel(ETLogger::Level::Silent);
    logger->ET_logMessage(ETLogger::Level::Debug, "D");
    logger->ET_logMessage(ETLogger::Level::Info, "I");
    logger->ET_logMessage(ETLogger::Level::Warning, "W");
    logger->ET_logMessage(ETLogger::Level::Error, "E");
    logger->ET_logMessage(ETLogger::Level::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "");
}

TEST_F(LoggerTests, CheckLogThroughET) {
    logger->ET_setLogLevel(ETLogger::Level::Debug);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}