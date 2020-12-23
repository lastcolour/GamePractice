#ifndef __ET_SYNCHRONIZATION_HPP__
#define __ET_SYNCHRONIZATION_HPP__

#include "Core/TypeId.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <atomic>

class ETSyncRoute {
public:

    ETSyncRoute(int etNodesCount);
    ~ETSyncRoute();

    bool tryBlockRoute(int reqRouteId);
    void unlockRoute(int reqRouteId);

    void pushRoute(int reqRouteId);
    void popRoute(int reqRouteId);
    bool popAndBlock(int reqRouteId);

    bool tryHardBlock(int reqRouteId);
    void hardUnlock(int reqRouteId);

private:

    struct ThreadData {
        int count { 0 };
        std::thread::id threadId;
    };

    struct Node {
        ThreadData data[4];
        std::atomic<bool> blocked;
    };

private:

    bool isBlockable(const Node& node) const;
    bool isUniqueThread(const Node& node, const std::thread::id& threadId) const;
    void addThread(Node& node, const std::thread::id& threadId);
    void removeThread(Node& node, const std::thread::id& threadId);

private:

    std::vector<Node> blockedRouteMap;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */