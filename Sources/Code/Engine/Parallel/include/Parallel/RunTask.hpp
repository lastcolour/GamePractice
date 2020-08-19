#ifndef __RUN_TASK_HPP__
#define __RUN_TASK_HPP__

#include <vector>
#include <functional>

enum class RunTaskType {
    Default = 0,
    MainThreadOnly,
    NoInMainThread,
};

class RunTask {
public:

    using CallT = std::function<void(void)>;

public:

    RunTask(const std::string& name, CallT callFunc);
    ~RunTask();

    void setRunCount(int newRunCount);
    int getRunCount() const;
    void setType(RunTaskType newType);
    RunTaskType getType() const;
    void addChild(std::unique_ptr<RunTask>& other);
    std::vector<RunTask*>& getChildren();

    void execute();

private:

    std::vector<RunTask*> childrenTasks;
    std::string name;
    CallT func;
    RunTaskType type;
    int runCount;
};

#endif /* __RUN_TASK_HPP__ */