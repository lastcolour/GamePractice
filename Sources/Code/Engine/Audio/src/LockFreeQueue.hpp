#ifndef __LOCK_FREE_QUEUE_HPP__
#define __LOCK_FREE_QUEUE_HPP__

#include <atomic>

class LockFreeQueue {
public:

    LockFreeQueue();
    ~LockFreeQueue();

    void init(size_t capacity);
    void* peek();
    void pop();
    void push(void* elem);
    size_t capacity() const;
    size_t size() const;
    bool empty() const;
    bool full() const;

private:

    LockFreeQueue(const LockFreeQueue&) = delete;
    LockFreeQueue& operator=(const LockFreeQueue&) = delete;

private:

    std::atomic<size_t> head;
    std::atomic<size_t> tail;
    std::vector<void*> elems;
};

#endif /* __LOCK_FREE_QUEUE_HPP__ */