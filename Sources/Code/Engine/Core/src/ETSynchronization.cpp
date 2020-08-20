#include "ETSynchronization.hpp"

#include <cassert>
#include <algorithm>

ETSyncRoute::ETSyncRoute(int etNodesCount) {
    assert(etNodesCount > 0 && "Invalud ET nodes count");
    blockedRouteMap.reset(new Node[etNodesCount]);
}

ETSyncRoute::~ETSyncRoute() {
}

bool ETSyncRoute::pushRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    node.mutex.lock();
    node.count += 1;
    return node.count == 1;
}

void ETSyncRoute::popRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    node.count -= 1;
    node.mutex.unlock();
}

bool ETSyncRoute::tryPushUniqueRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    if(!node.mutex.try_lock()) {
        return false;
    }
    if(node.count == 0) {
        node.count += 1;
        return true;
    }
    node.mutex.unlock();
    return false;
}