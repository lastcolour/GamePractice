#include "LoggerTests.hpp"
#include "Platform/PlatformLogger.hpp"

namespace {

const char* TEST_LOG_MSG = "TestMessage";

} // namespace

class TestLogger : public PlatformLogger {
public:
    virtual ~TestLogger() = default;

    const char* getLoggedMessage() const {
        return loggedMsg.c_str();
    }

protected:

    void printMessasge(LogLevel logLevel, const std::string& msg) override {
        loggedMsg += msg;
    }

    std::string formatMessage(LogLevel logLevel, const std::string& msg) override {
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
    logger->ET_setLogLevel(LogLevel::Debug);
    logger->ET_logMessage(LogLevel::Debug, "D");
    logger->ET_logMessage(LogLevel::Info, "I");
    logger->ET_logMessage(LogLevel::Warning, "W");
    logger->ET_logMessage(LogLevel::Error, "E");
    logger->ET_logMessage(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}

TEST_F(LoggerTests, CheckLogInfo) {
    logger->ET_setLogLevel(LogLevel::Info);
    logger->ET_logMessage(LogLevel::Debug, "D");
    logger->ET_logMessage(LogLevel::Info, "I");
    logger->ET_logMessage(LogLevel::Warning, "W");
    logger->ET_logMessage(LogLevel::Error, "E");
    logger->ET_logMessage(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "IWEF");
}

TEST_F(LoggerTests, CheckLogWarning) {
    logger->ET_setLogLevel(LogLevel::Warning);
    logger->ET_logMessage(LogLevel::Debug, "D");
    logger->ET_logMessage(LogLevel::Info, "I");
    logger->ET_logMessage(LogLevel::Warning, "W");
    logger->ET_logMessage(LogLevel::Error, "E");
    logger->ET_logMessage(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "WEF");
}

TEST_F(LoggerTests, CheckLogError) {
    logger->ET_setLogLevel(LogLevel::Error);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "EF");
}

TEST_F(LoggerTests, CheckLogFatal) {
    logger->ET_setLogLevel(LogLevel::Fatal);
    logger->ET_logMessage(LogLevel::Debug, "D");
    logger->ET_logMessage(LogLevel::Info, "I");
    logger->ET_logMessage(LogLevel::Warning, "W");
    logger->ET_logMessage(LogLevel::Error, "E");
    logger->ET_logMessage(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "F");
}

TEST_F(LoggerTests, CheckLogSilent) {
    logger->ET_setLogLevel(LogLevel::Silent);
    logger->ET_logMessage(LogLevel::Debug, "D");
    logger->ET_logMessage(LogLevel::Info, "I");
    logger->ET_logMessage(LogLevel::Warning, "W");
    logger->ET_logMessage(LogLevel::Error, "E");
    logger->ET_logMessage(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "");
}

TEST_F(LoggerTests, CheckLogThroughET) {
    logger->ET_setLogLevel(LogLevel::Debug);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}