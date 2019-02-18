#include "LoggerTests.hpp"
#include "Logger.hpp"
#include "Platforms/LogStream.hpp"
#include "TestUtils/VoidTestApplication.hpp"
#include "Platforms/Desktop/DesktopPlatform.hpp"

namespace {

    const char* TEST_LOG_MSG = "TestMessage";

    class TestLogStream : public LogStream {
    public:

        void logMessage(LogLevel logLvl, const std::string& msg) override {
            logged += msg;
        }
    
    public:

        std::string logged;
    };
}

void LoggerTests::SetUp() {
    stream = new TestLogStream;
    logger.reset(new Logger(std::unique_ptr<LogStream>(stream)));
    logger->setLogLevel(LogLevel::Debug);
}

void LoggerTests::TearDown() {
    logger.reset();
    stream = nullptr;
}

const char* LoggerTests::getLoggedMessage() const {
    assert(stream && "Invalid stream");
    return (static_cast<TestLogStream*>(stream))->logged.c_str();
}

TEST_F(LoggerTests, CheckLogEmptyMessage) {
    logger->message(LogLevel::Debug, "");
    ASSERT_STREQ(getLoggedMessage(), "");
}

TEST_F(LoggerTests, CheckLogDebug) {
    logger->setLogLevel(LogLevel::Debug);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "DIWEF");
}

TEST_F(LoggerTests, CheckLogInfo) {
    logger->setLogLevel(LogLevel::Info);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "IWEF");
}

TEST_F(LoggerTests, CheckLogWarning) {
    logger->setLogLevel(LogLevel::Warning);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "WEF");
}

TEST_F(LoggerTests, CheckLogError) {
    logger->setLogLevel(LogLevel::Error);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "EF");
}

TEST_F(LoggerTests, CheckLogFatal) {
    logger->setLogLevel(LogLevel::Fatal);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "F");
}

TEST_F(LoggerTests, CheckLogSilent) {
    logger->setLogLevel(LogLevel::Silent);
    logger->message(LogLevel::Debug, "D");
    logger->message(LogLevel::Info, "I");
    logger->message(LogLevel::Warning, "W");
    logger->message(LogLevel::Error, "E");
    logger->message(LogLevel::Fatal, "F");
    ASSERT_STREQ(getLoggedMessage(), "");
}

TEST_F(LoggerTests, CheckNormalFormats) {
    const std::string s = "1";
    std::string str = StringFormat("%i %.1f %s %s", 1, 1.0f, "1", s);
    ASSERT_STREQ(str.c_str(), "1 1.0 1 1");
}

TEST_F(LoggerTests, CheckVoidFormats) {
    const std::string testStr = "test_str";
    std::string resStr = StringFormat(testStr);
    ASSERT_EQ(resStr, testStr);
}

TEST_F(LoggerTests, CheckLoggerInEnv) {
    VoidTestApplication app(new DesktopPlatform(0, nullptr));
    auto factory = reinterpret_cast<VoidTestModuleFactory*>(app.retRes_createModuleFactory.get());
    VoidTestLogger* logger = factory->retRes_createLogger.get();

    ASSERT_TRUE(app.init());
    ASSERT_EQ(GetEnv()->getLogger(), logger);
}