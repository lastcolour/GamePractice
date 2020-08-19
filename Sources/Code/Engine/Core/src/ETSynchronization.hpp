#ifndef __ET_SYNCHRONIZATION_HPP__
#define __ET_SYNCHRONIZATION_HPP__

#include "Core/TypeId.hpp"
#include "ThreadStack.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

class ETSyncRoute {
private:

    using ThreadRouteMapT = std::unordered_map<std::thread::id, ThreadStack>;

public:

    ETSyncRoute();
    ~ETSyncRoute();

    bool tryPushUniqueRoute(TypeId reqRouteId);
    void pushRoute(TypeId reqRouteId);
    void popRoute();
    bool isRouteUniqueForCurrentThread(TypeId reqRouteId) const;

private:

    bool isRouteSafeForCurrentThread(TypeId reqRouteId) const;

private:

    ThreadRouteMapT routesMap;
    std::mutex routeMutex;
    std::condition_variable cond;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */