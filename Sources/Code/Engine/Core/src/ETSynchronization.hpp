#ifndef __ET_SYNCHRONIZATION_HPP__
#define __ET_SYNCHRONIZATION_HPP__

#include "Core/TypeId.hpp"
#include "ThreadStack.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <condition_variable>
#include <atomic>

class ETSyncRoute {
private:

    using ThreadRouteMapT = std::unordered_map<std::thread::id, ThreadStack>;

public:

    ETSyncRoute(int etNodesCount);
    ~ETSyncRoute();

    bool tryPushUniqueRoute(int reqRouteId);
    bool pushRoute(int reqRouteId);
    void popRoute(int reqRouteId);

private:

    struct Node {
        std::recursive_mutex mutex;
        int count;

        Node() : count(0) {}
    };

private:

    std::unique_ptr<Node[]> blockedRouteMap;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */