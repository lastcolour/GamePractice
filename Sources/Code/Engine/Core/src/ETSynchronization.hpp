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

    bool blockRoute(int reqRouteId);
    void unlockRoute(int reqRouteId);

    void pushRoute(int reqRouteId);
    void popRoute(int reqRouteId);

    void softBlock(int reqRouteId);
    void softUnlock(int reqRouteId);

private:

    struct ThreadData {
        int count { 0 };
        std::thread::id threadId;
    };

    struct Node {
        bool blocked {false};
        ThreadData data[4];
    };

private:

    bool isBlockable(const Node& node) const;
    bool isUniqueThread(const Node& node, const std::thread::id& threadId) const;
    void addThread(Node& node, const std::thread::id& threadId);
    void removeThread(Node& node, const std::thread::id& threadId);
    int getThreadId(const Node& node, const std::thread::id& threadId) const;

private:

    std::mutex mutex;
    std::condition_variable cond;
    std::vector<Node> blockedRouteMap;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */