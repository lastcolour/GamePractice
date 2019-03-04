#include "LoggerTests.hpp"
#include "TestUtils/ConsoleAppTests.hpp"
#include "Platforms/Desktop/DesktopLogger.hpp"

namespace {

    const char* TEST_LOG_MSG = "TestMessage";
}

class TestLogger : public DesktopLogger {
public:
    virtual ~TestLogger() = default;

    const char* getLoggedMessage() const {
        return loggedMsg.c_str();
    }

protected:

    // SystemLogic
    bool onInit() {
        ETNode<ETLogger>::connect(getEntityId());
    }

    void printMessasge(LogLevel logLevel, const std::string& msg) {
        loggedMsg += msg;
    }

    std::string formatMessage(LogLevel logLevel, const std::string& msg) {
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
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}

TEST_F(LoggerTests, CheckLogInfo) {
    logger->ET_setLogLevel(LogLevel::Info);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "IWEF");
}

TEST_F(LoggerTests, CheckLogWarning) {
    logger->ET_setLogLevel(LogLevel::Warning);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
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
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "F");
}

TEST_F(LoggerTests, CheckLogSilent) {
    logger->ET_setLogLevel(LogLevel::Silent);
    LogDebug("D");
    LogInfo("I");
    LogWarning("W");
    LogError("E");
    LogFatal("F");
    ASSERT_STREQ(getLoggedMessage(), "");
}