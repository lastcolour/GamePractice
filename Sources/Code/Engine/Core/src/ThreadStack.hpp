#ifndef __THREAD_STACK_HPP__
#define __THREAD_STACK_HPP__

#include "Core/TypeId.hpp"

#include <thread>

class ThreadStack {
public:

    ThreadStack();
    ThreadStack(ThreadStack& other);
    ThreadStack& operator=(ThreadStack&& other);
    ~ThreadStack();

    void pop();
    void push(TypeId typeId);
    bool contain(TypeId typeId) const;
    int count(TypeId typeId) const;

    void startWait(TypeId typeId);
    void stopWait();

    bool isDeadLock(const ThreadStack& other) const;

private:

    TypeId waitingRoute;
    std::vector<TypeId> stack;
};

#endif /* __THREAD_STACK_HPP__ */