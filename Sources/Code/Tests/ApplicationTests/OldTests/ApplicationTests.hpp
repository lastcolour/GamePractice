#ifndef __APPLICATION_TESTS_HPP__
#define __APPLICATION_TESTS_HPP__

#include <gtest/gtest.h>

#include <condition_variable>
#include <mutex>

class Application;

class ApplicationTests : public ::testing::Test {
protected:

    ApplicationTests();
    virtual ~ApplicationTests();

    void SetUp();
    void TearDown();

    bool startAppThread(Application& app);
    bool isAppFinished();

protected:

    std::condition_variable cvVar;
    std::mutex mutex;
    int appRes;
    bool appFinished;
};

#endif /* __APPLICATION_TESTS_HPP__ */