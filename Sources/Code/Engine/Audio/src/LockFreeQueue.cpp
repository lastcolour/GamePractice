#include "LockFreeQueue.hpp"

#include <cassert>

LockFreeQueue::LockFreeQueue() {
}

LockFreeQueue::~LockFreeQueue() {
}

void LockFreeQueue::init(unsigned int capacity) {
    tail.store(0u);
    head.store(0u);
    elems.clear();
    elems.resize(capacity, nullptr);
}

void* LockFreeQueue::peek() {
    if(empty()) {
        return nullptr;
    }
    unsigned int h = head.load();
    return elems[h % capacity()];
}

void LockFreeQueue::pop() {
    assert(!empty() && "Queue empty");
    int h = head.load();
    h = h + 1;
    head.store(h);
}

void LockFreeQueue::push(void* elem) {
    assert(!full() && "Queue overflow");

    int t = tail.load();
    elems[t % capacity()] = elem;
    t = t + 1;
    tail.store(t);
}

unsigned int LockFreeQueue::capacity() const {
    return elems.size();
}

unsigned int LockFreeQueue::size() const {
    auto t = tail.load();
    auto h = head.load();
    return t - h;
}

bool LockFreeQueue::empty() const {
    int t = tail.load();
    int h = head.load();
    return t == h;
}

bool LockFreeQueue::full() const {
    return size() == capacity();
}