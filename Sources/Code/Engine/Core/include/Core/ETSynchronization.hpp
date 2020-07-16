#ifndef __ET_SYNCHRONIZATION_HPP__
#define __ET_SYNCHRONIZATION_HPP__

#include "Core/TypeId.hpp"

#include <vector>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <condition_variable>

class ETSyncRoute {
private:

    using ThreadIdT = std::thread::id;
    using RouteArrayT = std::vector<TypeId>;
    using ThreadRouteMapT = std::unordered_map<ThreadIdT, RouteArrayT>;

public:

    ETSyncRoute();
    ~ETSyncRoute();

    bool tryPushUniqueRoute(TypeId reqRouteId);
    void pushRoute(TypeId reqRouteId);
    void popRoute();
    bool isRouteUniqueForCurrentThread(TypeId reqRouteId) const;

private:

    bool isRouteSafeForCurrentThread(TypeId reqRouteId) const;
    void addRouteForCurrentThread(TypeId reqRouteId);
    void popRouteForCurrentThread();
    size_t getRountCountForCurrentThread(TypeId reqRouteId) const;

private:

    ThreadRouteMapT routesMap;
    std::mutex routeMutex;
    std::condition_variable cond;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */