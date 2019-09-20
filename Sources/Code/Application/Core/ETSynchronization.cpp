#include "Core/ETSynchronization.hpp"

#include <cassert>

ETSyncRoute::ETSyncRoute() {
}

ETSyncRoute::~ETSyncRoute() {
}

void ETSyncRoute::addRouteForCurrentThread(TypeId reqRouteId) {
    auto threadId = std::this_thread::get_id();
    auto it = routesMap.find(threadId);
    RouteArrayT* activeRoute = nullptr;
    if(it == routesMap.end()) {
        activeRoute = &(routesMap[threadId]);
    } else {
        activeRoute = &(it->second);
    }
    activeRoute->push_back(reqRouteId);
}

void ETSyncRoute::popRouteForCurrentThread() {
    auto threadId = std::this_thread::get_id();
    auto it = routesMap.find(threadId);
    assert(it != routesMap.end() && "Invalid thread id");
    RouteArrayT* activeRoute = &(it->second);
    activeRoute->pop_back();
}

void ETSyncRoute::pushRoute(TypeId reqRouteId) {
    std::unique_lock<std::mutex> lock(routeMutex);
    cond.wait(lock, [this, reqRouteId](){
        return isRouteSafeForCurrentThread(reqRouteId);
    });
    addRouteForCurrentThread(reqRouteId);
}

void ETSyncRoute::popRoute() {
    std::lock_guard<std::mutex> lock(routeMutex);
    popRouteForCurrentThread();
    cond.notify_all();
}

bool ETSyncRoute::isRouteBlocked(TypeId reqRouteId) const {
    (void)reqRouteId;
    return false;
}

bool ETSyncRoute::isRouteSafeForCurrentThread(TypeId reqRouteId) const {
    if(isRouteBlocked(reqRouteId)) {
        return false;
    }
    auto threadId = std::this_thread::get_id();
    for(const auto& routeNode : routesMap) {
        if(routeNode.first != threadId) {
            for(auto routeId : routeNode.second) {
                if(routeId == reqRouteId) {
                    return false;
                }
            }
        }
        
    }
    return true;
}

bool ETSyncRoute::tryBlockRoute(TypeId reqRouteId) {
    if(isRouteSafeForCurrentThread(reqRouteId)) {

    }
    (void)reqRouteId;
    return true;
}

void ETSyncRoute::unlockRoute() {
    return;
}