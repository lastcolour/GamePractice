#include "ETSyncTests.hpp"
#include "TestUtils/FutureTimePoint.hpp"
#include "ETSynchronization.hpp"

#include <atomic>
#include <thread>

namespace {

const int THREAD_OP_WAIT_DELAY_MS = 250;

} // namespace

void ETSyncTests::stopFristThread() {
    bool is_t1_running = true;
    FutureTimePoint timeP(THREAD_OP_WAIT_DELAY_MS);
    t1_needRun.store(false);
    while(!timeP.isHappened()) {
        if(!t1_run.load()) {
            is_t1_running = false;
            break;
        } else {
            std::this_thread::yield();
        }
    }
    ASSERT_FALSE(is_t1_running);
}

void ETSyncTests::stopSecondThread() {
    bool is_t2_running = true;
    FutureTimePoint timeP(THREAD_OP_WAIT_DELAY_MS);
    t2_needRun.store(false);
    while(!timeP.isHappened()) {
        if(!t2_run.load()) {
            is_t2_running = false;
            break;
        } else {
            std::this_thread::yield();
        }
    }
    ASSERT_FALSE(is_t2_running);
}

bool ETSyncTests::isFirstThreadRunning() {
    bool is_t1_running = false;
    FutureTimePoint timeP(THREAD_OP_WAIT_DELAY_MS);
    while(!timeP.isHappened()) {
        if(t1_run.load()) {
            is_t1_running = true;
            break;
        } else {
            std::this_thread::yield();
        }
    }
    return is_t1_running;
}

bool ETSyncTests::isSecondThreadRunning() {
    bool is_t2_running = false;
    FutureTimePoint timeP(THREAD_OP_WAIT_DELAY_MS);
    while(!timeP.isHappened()) {
        if(t2_run.load()) {
            is_t2_running = true;
            break;
        } else {
            std::this_thread::yield();
        }
    }
    return is_t2_running;
}

TEST_F(ETSyncTests, CheckSeparateRoutes) {
    ETSyncRoute syncRoute(10);

    {
        std::thread t1([&syncRoute, this](){
            syncRoute.pushRoute(1);
            t1_run.store(true);
            while(t1_needRun.load()) {
                std::this_thread::yield();
            }
            syncRoute.popRoute(1);
            t1_run.store(false);
        });
        t1.detach();
    }

    {
        std::thread t2([&syncRoute, this](){
            syncRoute.pushRoute(2);
            t2_run.store(true);
            while(t2_needRun.load()) {
                std::this_thread::yield();
            }
            syncRoute.popRoute(2);
            t2_run.store(false);
        });
        t2.detach();
    }

    EXPECT_TRUE(isFirstThreadRunning());
    EXPECT_TRUE(isSecondThreadRunning());

    stopFristThread();
    stopSecondThread();
}

void ETSyncTests::SetUp() {
    t1_run.store(false);
    t1_needRun.store(true);

    t2_run.store(false);
    t2_needRun.store(true);
}

TEST_F(ETSyncTests, CheckIntersectRoutes) {
    ETSyncRoute syncRoute(10);

    {
        std::thread t1([&syncRoute, this](){
            syncRoute.pushRoute(1);
            t1_run.store(true);
            while(t1_needRun.load()) {
                std::this_thread::yield();
            }
            syncRoute.popRoute(1);
            t1_run.store(false);
        });
        t1.detach();
    }

    EXPECT_TRUE(isFirstThreadRunning());

    {
        std::thread t2([&syncRoute, this](){
            syncRoute.pushRoute(1);
            t2_run.store(true);
            while(t2_needRun.load()) {
                std::this_thread::yield();
            }
            syncRoute.popRoute(1);
            t2_run.store(false);
        });
        t2.detach();
    }

    EXPECT_TRUE(isSecondThreadRunning());

    stopFristThread();

    EXPECT_TRUE(isSecondThreadRunning());

    stopSecondThread();
}