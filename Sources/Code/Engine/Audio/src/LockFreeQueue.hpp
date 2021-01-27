#ifndef __LOCK_FREE_QUEUE_HPP__
#define __LOCK_FREE_QUEUE_HPP__

#include <atomic>
#include <vector>

class LockFreeQueue {
public:

    LockFreeQueue();
    ~LockFreeQueue();

    void init(unsigned int capacity);
    void* peek();
    void pop();
    void push(void* elem);
    unsigned int capacity() const;
    unsigned int size() const;
    bool empty() const;
    bool full() const;

private:

    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

private:

    std::atomic<unsigned int> head;
    std::atomic<unsigned int> tail;
    std::vector<void*> elems;
};

#endif /* __LOCK_FREE_QUEUE_HPP__ */