#ifndef __RUN_TASK_HPP__
#define __RUN_TASK_HPP__

#include <vector>
#include <functional>

enum class RunTaskType {
    Default = 0,
    MainThreadOnly,
    NoInMainThread,
};

enum class RunState {
    Waiting = 0,
    Running
};

class RunTask {
public:

    using CallT = std::function<void(void)>;

public:

    RunTask(CallT callFunc);
    RunTask(const RunTask& other);
    RunTask& operator=(const RunTask& other);
    ~RunTask();

    void setType(RunTaskType newType);
    bool canStart(int threadId) const;
    void onStarted();
    void onFinished();
    int getRunCount() const;
    void execute();

private:

    CallT func;
    RunTaskType type;
    RunState state;
    int frameId;
    int runCount;
};

#endif /* __RUN_TASK_HPP__ */