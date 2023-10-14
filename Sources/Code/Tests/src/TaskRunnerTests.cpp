#include "TaskRunnerTests.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Parallel/RunTask.hpp"
#include "Core/TimePoint.hpp"
#include "Math/Random.hpp"
#include "Parallel/src/ThreadJob.hpp"

#include <thread>
#include <atomic>

TEST_F(TaskRunnerTests, CheckInitialPendingCounts) {
    TasksRunner runner;

    auto task1 = runner.createTask("Task1", [](){
    });

    auto task2 = runner.createTask("Task2", [](){

    });
    auto task3 = runner.createTask("Task3", [](){

    });

    task1->addChild(task2);
    task1->addChild(task3);
    task2->addChild(task3);

    int runCount = 0;
    runner.runUntil(1, [&runCount](){
        runCount += 1;
        return runCount <= 1;
    });

    EXPECT_EQ(task1->getThreadJob()->getPendingCount(), 0);
    EXPECT_EQ(task2->getThreadJob()->getPendingCount(), 1);
    EXPECT_EQ(task3->getThreadJob()->getPendingCount(), 2);
}

TEST_F(TaskRunnerTests, SimpleOneTimeTaskOneThread) {
    TasksRunner runner;
    int value = 0;
    runner.createTask("Test", [&value](){
        ++value;
        std::this_thread::yield();
    });
    runner.runUntil(1, [&value](){
        return value < 1;
    });

    EXPECT_EQ(value, 1);
}

TEST_F(TaskRunnerTests, RunTwoTasksTwoThreads) {
    std::atomic<int> value = 0;
    TasksRunner runner;

    runner.createTask("Test1", [&value](){
        value.fetch_add(1);
        std::this_thread::yield();
    });
    runner.createTask("Test2", [&value](){
        value.fetch_add(1);
        std::this_thread::yield();
    });

    runner.runUntil(2, [&value](){
        return value.load() < 100;
    });

    EXPECT_GE(value.load(), 100);
    EXPECT_LE(value.load(), 110);
}

TEST_F(TaskRunnerTests, RunTenTasksTenThreads) {
    std::atomic<int> value = 0;
    TasksRunner runner;
    for(int i = 0; i < 10; ++i) {
        runner.createTask("Test", [&value, i](){
            value.fetch_add(1);
            std::this_thread::yield();
        })->setFrequency(512);
    }
    runner.runUntil(10, [&value](){
        return value.load() < 10000;
    });

    int totalRunCount = 0;
    TaskRunInfo runInfo;
    for(auto& task : runner.getTasks()) {
        EXPECT_TRUE(runner.getTaskRunInfo(*task, runInfo));
        EXPECT_GT(runInfo.runCount, 50);
        totalRunCount += runInfo.runCount;
    }
    EXPECT_EQ(value.load(), totalRunCount);
    EXPECT_GE(value.load(), 10000);
    EXPECT_LE(value.load(), 10200);
}

TEST_F(TaskRunnerTests, TestFrequency) {
    int TEST_FREQUENCY = 200;

    std::atomic<int> value = 0;
    TasksRunner runner;
    auto task = runner.createTask("Test", [&value](){
        ++value;
        std::this_thread::yield();
    });
    task->setFrequency(TEST_FREQUENCY);

    TimePoint startTime = TimePoint::GetNow();
    runner.runUntil(1, [&value, &startTime](){
        if(value.load() == 1) {
            startTime = TimePoint::GetNow();
            return true;
        } else {
            return startTime.getSecDeltaWithNow() < 1.f;
        }
    });

    TaskRunInfo runInfo;
    EXPECT_TRUE(runner.getTaskRunInfo(*task, runInfo));

    EXPECT_EQ(runInfo.runCount, value.load());
    EXPECT_GE(value.load(), TEST_FREQUENCY - 1);
}

TEST_F(TaskRunnerTests, RunTaskWithChildren) {
    std::atomic<int> value(0);
    std::atomic<int> botValue(0);

    TasksRunner runner;

    auto topTask = runner.createTask("Top", [&value](){
        value.store(5);
    });
    auto leftTask = runner.createTask("Left", [&value](){
        value.fetch_sub(2);
    });
    auto rightTask = runner.createTask("Right", [&value](){
        value.fetch_sub(2);
    });
    auto botTask = runner.createTask("Bot", [&value, &botValue](){
        botValue.fetch_add(value.load());
    });

    topTask->addChild(leftTask);
    topTask->addChild(rightTask);
    leftTask->addChild(botTask);
    rightTask->addChild(botTask);

    runner.runUntil(4, [&botValue](){
        return botValue.load() < 500;
    });

    TaskRunInfo runInfo;
    for(auto& task : runner.getTasks()) {
        EXPECT_TRUE(runner.getTaskRunInfo(*task, runInfo));
        EXPECT_LE(runInfo.runCount, botValue.load() + 1);
    }
}

TEST_F(TaskRunnerTests, CheckManualMainThreadStepping) {
    int MAX_ITER = 10000;
    std::atomic<int> value(0);
    std::atomic<int> auxValue(0);

    TasksRunner runner;

    auto mainTask = runner.createTask("Main", [&value](){
        value.fetch_add(1);
    });
    mainTask->setType(RunTaskType::MainThreadOnly);

    auto auxTask = runner.createTask("Aux", [&auxValue](){
        auxValue.fetch_add(1);
    });
    auxTask->setType(RunTaskType::NoInMainThread);

    runner.startOtherThreads(1);
    for(int i = 0; i < MAX_ITER; ++i) {
        runner.stepMainThread();
    }
    runner.stopOtherTreads();

    EXPECT_EQ(value.load(), MAX_ITER);
    EXPECT_GE(auxValue.load(), MAX_ITER / 3);
}

TEST_F(TaskRunnerTests, CheckSuspendRunning) {
    TasksRunner runner;

    std::atomic<bool> runFlag(true);
    std::atomic<int> val(0);

    runner.createTask("Test", [&val](){
        val.fetch_add(1);
    });

    std::thread runThread([&runner, &runFlag](){
        runner.runUntil(1, [&runFlag](){
            return runFlag.load();
        });
    });

    while(val.load() == 0) {
        std::this_thread::yield();
    }

    runner.suspend(true);
    std::this_thread::yield();

    {
        int first = val.load();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int second = val.load();

        EXPECT_EQ(first, second);
    }

    runner.suspend(false);

    {
        int first = val.load();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        int second = val.load();

        EXPECT_NE(first, second);
    }

    runFlag.store(false);
    runThread.join();
}

TEST_F(TaskRunnerTests, CheckNoIntersections) {
    TasksRunner runner;

    std::atomic<int> bRunning[201];
    for(int i = 0; i < 201; ++i) {
        bRunning[i].store(0);
    }

    std::atomic<int> cRunning[201];
    for(int i = 0; i < 201; ++i) {
        cRunning[i].store(0);
    }

    int loopCount = 0;
    const int runFrequency = 240;

    auto taskA = runner.createTask("A", [&bRunning, &cRunning, &loopCount](){
        for(int i = 0; i < 201; ++i) {
            ASSERT_EQ(bRunning[i].load(), 0);
            ASSERT_EQ(cRunning[i].load(), 0);
        }
        ++loopCount;
    });

    taskA->setFrequency(runFrequency);

    Math::RandomIntGenerator bIntGen(0, 200);
    auto taskB = runner.createTask("B", [&bRunning, &bIntGen](){
        const int randInt = bIntGen.generate();
        bRunning[randInt] += 1;
        std::this_thread::sleep_for(std::chrono::microseconds(250 - randInt));
        bRunning[randInt] -= 1;
    });
    taskB->setFrequency(runFrequency);

    Math::RandomIntGenerator cIntGen(0, 200);
    auto taskC = runner.createTask("C", [&cRunning, &cIntGen](){
        const int randInt = cIntGen.generate();
        cRunning[randInt] += 1;
        std::this_thread::sleep_for(std::chrono::microseconds(250 - randInt));
        cRunning[randInt] -= 1;
    });
    taskC->setFrequency(runFrequency);

    taskA->addChild(taskB);
    taskA->addChild(taskC);

    taskB->addChild(taskC);

    const float runDuration = 5.f;
    auto startT = TimePoint::GetNow();
    runner.runUntil(4, [&startT, runDuration](){
        return startT.getSecDeltaWithNow() < runDuration;
    });

    const int expectedLoopCount = runFrequency * runDuration - 1;
    EXPECT_GE(loopCount, expectedLoopCount);
}