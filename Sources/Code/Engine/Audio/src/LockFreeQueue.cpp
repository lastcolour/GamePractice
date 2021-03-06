#include "LockFreeQueue.hpp"

#include <cassert>

LockFreeQueue::LockFreeQueue() {
}

LockFreeQueue::~LockFreeQueue() {
}

void LockFreeQueue::init(size_t capacity) {
    tail.store(0u);
    head.store(0u);
    elems.clear();
    elems.resize(capacity, nullptr);
}

void* LockFreeQueue::peek() {
    if(empty()) {
        return nullptr;
    }
    auto h = head.load();
    return elems[h % capacity()];
}

void LockFreeQueue::pop() {
    assert(!empty() && "Queue empty");

    auto h = head.load();
    h = h + 1;
    head.store(h);
}

void LockFreeQueue::push(void* elem) {
    assert(!full() && "Queue overflow");

    auto t = tail.load();
    elems[t % capacity()] = elem;
    t = t + 1;
    tail.store(t);
}

size_t LockFreeQueue::capacity() const {
    return elems.size();
}

size_t LockFreeQueue::size() const {
    auto t = tail.load();
    auto h = head.load();
    return t - h;
}

bool LockFreeQueue::empty() const {
    auto t = tail.load();
    auto h = head.load();
    return t == h;
}

bool LockFreeQueue::full() const {
    return size() == capacity();
}