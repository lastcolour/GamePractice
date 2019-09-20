#ifndef __ET_SYNC_TESTS_HPP__
#define __ET_SYNC_TESTS_HPP__

#include <gtest/gtest.h>

#include <atomic>

class ETSyncTests : public ::testing::Test {
protected:

    void stopFristThread();
    void stopSecondThread();

    bool isFirstThreadRunning();
    bool isSecondThreadRunning();

    void SetUp() override;

protected:

    std::atomic<bool> t1_run;
    std::atomic<bool> t1_needRun;

    std::atomic<bool> t2_run;
    std::atomic<bool> t2_needRun;
};

#endif /* __ET_SYNC_TESTS_HPP__ */