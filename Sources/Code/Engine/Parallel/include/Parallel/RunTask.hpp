#ifndef __RUN_TASK_HPP__
#define __RUN_TASK_HPP__

enum class RunTaskType {
    Default = 0,
    MainThreadOnly,
    NoInMainThread,
};

class RunTask {
public:

    using CallT = std::function<void(float)>;

public:

    RunTask(const char* name, CallT callFunc);
    ~RunTask();

    void addChild(RunTask* other);
    void setFrequency(int frequency);
    int getFrequency() const;
    void setType(RunTaskType newType);
    RunTaskType getType() const;
    const char* getName() const;
    std::vector<RunTask*>& getChildren();

    void execute(float dt);

private:

    std::vector<RunTask*> childrenTasks;
    std::string name;
    CallT func;
    int frequency;
    RunTaskType type;
};

#endif /* __RUN_TASK_HPP__ */