#include "ThreadStack.hpp"

#include <algorithm>
#include <cassert>

ThreadStack::ThreadStack() :
    waitingRoute(InvalidTypeId) {
}

ThreadStack::ThreadStack(ThreadStack& other) :
    waitingRoute(other.waitingRoute),
    stack(std::move(other.stack)) {
}

ThreadStack& ThreadStack::operator=(ThreadStack&& other) {
    if(this != &other) {
        waitingRoute = other.waitingRoute;
        stack = std::move(other.stack);
    }
    return *this;
}

ThreadStack::~ThreadStack() {
}

void ThreadStack::pop() {
    assert(!stack.empty() && "Can't pop from empty stack");
    stack.pop_back();
}

void ThreadStack::push(TypeId typeId) {
    stack.push_back(typeId);
}

bool ThreadStack::contain(TypeId typeId) const {
    auto it = std::find(stack.begin(), stack.end(), typeId);
    return it != stack.end();
}

int ThreadStack::count(TypeId typeId) const {
    auto res = std::count(stack.begin(), stack.end(), typeId);
    return static_cast<int>(res);
}

void ThreadStack::startWait(TypeId typeId) {
    waitingRoute = typeId;
}

void ThreadStack::stopWait() {
    waitingRoute = InvalidTypeId;
}

bool ThreadStack::isDeadLock(const ThreadStack& other) const {
    if(other.waitingRoute == InvalidTypeId) {
        return false;
    }
    if(waitingRoute == InvalidTypeId) {
        return false;
    }
    if(!other.contain(waitingRoute)) {
        return false;
    }
    if(!contain(other.waitingRoute)) {
        return false;
    }
    return true;
}