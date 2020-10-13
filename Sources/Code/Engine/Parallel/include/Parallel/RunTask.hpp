#ifndef __RUN_TASK_HPP__
#define __RUN_TASK_HPP__

#include <vector>
#include <functional>
#include <string>

enum class RunTaskType {
    Default = 0,
    MainThreadOnly,
    NoInMainThread,
};

class RunTask {
public:

    using CallT = std::function<void(void)>;

public:

    RunTask(const char* name, CallT callFunc);
    ~RunTask();

    void addChild(RunTask* other);
    void setFrequency(int frequency);
    int getFrequency() const;
    int getRunCount() const;
    void setType(RunTaskType newType);
    RunTaskType getType() const;
    const char* getName() const;
    std::vector<RunTask*>& getChildren();

    void execute();

private:

    std::vector<RunTask*> childrenTasks;
    std::string name;
    CallT func;
    RunTaskType type;
    int runCount;
    int frequency;
};

#endif /* __RUN_TASK_HPP__ */