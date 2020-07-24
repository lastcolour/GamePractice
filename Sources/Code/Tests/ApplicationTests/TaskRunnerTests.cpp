#include "TaskRunnerTests.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"

#include <thread>
#include <atomic>

TEST_F(TaskRunnerTests, SimpleOneTimeTaskOneThread) {
    int value = 0;
    RunTask task([&value](){
        ++value;
        std::this_thread::yield();
    });
    TasksRunner runner;
    runner.addTask(task);
    runner.runUntil(1, [&value](){
        return value < 1;
    });

    EXPECT_EQ(value, 1);
}

TEST_F(TaskRunnerTests, RunTwoTasksTwoThreads) {
    std::atomic<int> value = 0;
    RunTask task([&value](){
        value.fetch_add(1);
        std::this_thread::yield();
    });
    TasksRunner runner;
    runner.addTask(task);
    runner.addTask(task);
    runner.runUntil(2, [&value](){
        return value.load() < 100;
    });

    EXPECT_GE(value.load(), 100);
    EXPECT_LE(value.load(), 100);
}

TEST_F(TaskRunnerTests, RunTenTasksTenThreads) {
    std::atomic<int> value = 0;
    RunTask task([&value](){
        value.fetch_add(1);
        std::this_thread::yield();
    });
    TasksRunner runner;
    for(int i = 0; i < 10; ++i) {
        runner.addTask(task);
    }
    runner.runUntil(10, [&value](){
        return value.load() < 10000;
    });

    int totalRunCount = 0;
    for(auto& task : runner.getTasks()) {
        EXPECT_GT(task.getRunCount(), 50);
        totalRunCount += task.getRunCount();
    }
    EXPECT_EQ(value.load(), totalRunCount);
    EXPECT_GE(value.load(), 10000);
    EXPECT_LE(value.load(), 10010);
}