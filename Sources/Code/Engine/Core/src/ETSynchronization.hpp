#ifndef __ET_SYNCHRONIZATION_HPP__
#define __ET_SYNCHRONIZATION_HPP__

#include "Core/TypeId.hpp"
#include "ThreadStack.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>

class ETSyncRoute {
public:

    ETSyncRoute(int etNodesCount);
    ~ETSyncRoute();

    bool pushRoute(int reqRouteId);
    void popRoute(int reqRouteId);

private:

    struct Node {
        std::atomic<int> count;
        std::thread::id threadId;

        Node() : count(0) {}
    };

private:

    std::mutex mutex;
    std::condition_variable cond;
    std::vector<Node> blockedRouteMap;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */