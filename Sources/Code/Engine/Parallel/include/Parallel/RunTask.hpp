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
    void setTrackPerformance(bool flag);
    bool getTrackPerformance() const;
    int getFrequency() const;
    int getRunCount() const;
    void setType(RunTaskType newType);
    RunTaskType getType() const;
    const char* getName() const;
    std::vector<RunTask*>& getChildren();

    void execute(float dt);

private:

    std::vector<RunTask*> childrenTasks;
    std::string name;
    CallT func;
    int runCount;
    int frequency;
    RunTaskType type;
    bool trackPerformance;
};

#endif /* __RUN_TASK_HPP__ */