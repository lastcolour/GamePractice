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

    void pushRoute(TypeId reqRouteId);
    void popRoute();
    bool tryBlockRoute(TypeId reqRouteId);
    void unlockRoute();

private:

    bool isRouteSafeForCurrentThread(TypeId reqRouteId) const;
    void addRouteForCurrentThread(TypeId reqRouteId); 
    void popRouteForCurrentThread();
    bool isRouteBlocked(TypeId reqRouteId) const;

private:

    ThreadRouteMapT routesMap;
    std::vector<TypeId> blockedRoutes;
    std::mutex routeMutex;
    std::mutex blockMutex;
    std::condition_variable cond;
};

#endif /* __ET_SYNCHRONIZATION_HPP__ */