#include "Core/ETSynchronization.hpp"

#include <cassert>
#include <algorithm>

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

bool ETSyncRoute::isRouteSafeForCurrentThread(TypeId reqRouteId) const {
    auto blockedIt = std::find(blockedRoutes.begin(), blockedRoutes.end(), reqRouteId);
    if(blockedIt != blockedRoutes.end()) {
        return false;
    }
    auto threadId = std::this_thread::get_id();
    for(const auto& routeNode : routesMap) {
        if(routeNode.first != threadId) {
            auto it = std::find(routeNode.second.begin(), routeNode.second.end(), reqRouteId);
            if(it != routeNode.second.end()) {
                return false;
            }
        }
    }
    return true;
}

bool ETSyncRoute::tryBlockRoute(TypeId reqRouteId) {
    std::lock_guard<std::mutex> lock(routeMutex);
    for(const auto& routeNode : routesMap) {
        auto it = std::find(routeNode.second.begin(), routeNode.second.end(), reqRouteId);
        if(it != routeNode.second.end()) {
            return false;
        }
    }
    auto it = std::find(blockedRoutes.begin(), blockedRoutes.end(), reqRouteId);
    if(it != blockedRoutes.end()) {
        return false;
    }
    blockedRoutes.push_back(reqRouteId);
    return true;
}

void ETSyncRoute::unlockRoute(TypeId reqRouteId) {
    std::lock_guard<std::mutex> lock(routeMutex);
    auto it = std::find(blockedRoutes.begin(), blockedRoutes.end(), reqRouteId);
    assert(it != blockedRoutes.end() && "Invalid route id");
    blockedRoutes.erase(it);
    cond.notify_all();
}