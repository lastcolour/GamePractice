#include "ETSynchronization.hpp"

#include <cassert>
#include <algorithm>

ETSyncRoute::ETSyncRoute(int etNodesCount) :
    blockedRouteMap(etNodesCount) {
}

ETSyncRoute::~ETSyncRoute() {
}

bool ETSyncRoute::pushRoute(int reqRouteId) {
    bool isUnique = false;
    auto threadId = std::this_thread::get_id();
    int zero = 0;
    auto& node = blockedRouteMap[reqRouteId];
    {
        if(node.count.compare_exchange_strong(zero, 1)) {
            node.threadId = threadId;
            return true;
        }
    }
    {
        std::unique_lock<std::mutex> uniqueLock(mutex);
        if(node.threadId != threadId) {
            cond.wait(uniqueLock, [&node, &zero](){
                return node.count.compare_exchange_strong(zero, 1);
            });
            node.threadId = threadId;
            return true;
        }
    }
    node.count.fetch_add(1);
    return false;
}

void ETSyncRoute::popRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    node.count.fetch_sub(1);
    cond.notify_all();
}