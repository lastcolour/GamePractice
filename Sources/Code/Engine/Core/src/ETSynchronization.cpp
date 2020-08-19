#include "ETSynchronization.hpp"

#include <cassert>
#include <algorithm>

ETSyncRoute::ETSyncRoute() {
}

ETSyncRoute::~ETSyncRoute() {
}

void ETSyncRoute::pushRoute(TypeId reqRouteId) {
    std::unique_lock<std::mutex> lock(routeMutex);
    auto& stack = routesMap[std::this_thread::get_id()];
    stack.startWait(reqRouteId);
    cond.wait(lock, [this, reqRouteId]() {
        return isRouteSafeForCurrentThread(reqRouteId);
    });
    stack.stopWait();
    stack.push(reqRouteId);
}

void ETSyncRoute::popRoute() {
    {
        std::lock_guard<std::mutex> lock(routeMutex);
        auto it = routesMap.find(std::this_thread::get_id());
        assert(it != routesMap.end() && "Invalid thread id");
        auto& stack = it->second;
        stack.pop();
    }
    cond.notify_all();
}

bool ETSyncRoute::isRouteSafeForCurrentThread(TypeId reqRouteId) const {
    auto it = routesMap.find(std::this_thread::get_id());
    assert(it != routesMap.end() && "Can't find stack for thread");
    auto threadId = it->first;
    auto& routeStack = it->second;

    for(const auto& routeNode : routesMap) {
        if(routeNode.first != threadId) {
            auto& otherStack = routeNode.second;
            if(otherStack.isDeadLock(routeStack)) {
                assert(false && "Detected deadlock between two threads");
            }
        }
    }

    for(const auto& routeNode : routesMap) {
        if(routeNode.first != threadId) {
            auto& otherStack = routeNode.second;
            if(otherStack.contain(reqRouteId)) {
                return false;
            }
        }
    }
    return true;
}

bool ETSyncRoute::tryPushUniqueRoute(TypeId reqRouteId) {
    std::lock_guard<std::mutex> lock(routeMutex);
    auto& stack = routesMap[std::this_thread::get_id()];
    if(!isRouteSafeForCurrentThread(reqRouteId)) {
        return false;
    }
    if(stack.contain(reqRouteId)) {
        return false;
    }
    stack.push(reqRouteId);
    return true;
}

bool ETSyncRoute::isRouteUniqueForCurrentThread(TypeId reqRouteId) const {
    auto it = routesMap.find(std::this_thread::get_id());
    assert(it != routesMap.end() && "Can't find required route");
    return !it->second.contain(reqRouteId);
}