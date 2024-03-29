#include "ETSynchronization.hpp"

#include <cassert>

namespace ET {

ETSyncRoute::ETSyncRoute(int etNodesCount) :
    blockedRouteMap(etNodesCount) {
}

ETSyncRoute::~ETSyncRoute() {
}

bool ETSyncRoute::tryHardBlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    bool isBlocked = false;
    {
        while(true) {
            bool tFalse = false;
            if(node.blocked.compare_exchange_weak(tFalse, true)) {
                if(!isUniqueThread(node, threadId)) {
                   node.blocked.store(false);
                } else {
                    break;
                }
            }
            std::this_thread::yield();
        }
        isBlocked = isBlockable(node);
    }
    return isBlocked;
}

void ETSyncRoute::hardUnlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        assert(isUniqueThread(node, threadId) && "This thread isn't unique");
        assert(node.blocked.load() && "Node isn't blocked");
        node.blocked.store(false);
    }
}

void ETSyncRoute::pushRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        while(true) {
            bool tFalse = false;
            if(node.blocked.compare_exchange_weak(tFalse, true)) {
                addThread(node, threadId);
                node.blocked.store(false);
                break;
            }
            std::this_thread::yield();
        }
    }
}

void ETSyncRoute::popRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        while(true) {
            bool tFalse = false;
            if(node.blocked.compare_exchange_weak(tFalse, true)) {
                removeThread(node, threadId);
                node.blocked.store(false);
                break;
            }
            std::this_thread::yield();
        }
    }
}

bool ETSyncRoute::popAndBlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    bool isBlocaked = false;
    {
        while(true) {
            bool tFalse = false;
            if(node.blocked.compare_exchange_weak(tFalse, true)) {
                removeThread(node, threadId);
                if(isBlockable(node)) {
                    addThread(node, threadId);
                    isBlocaked = true;
                } else {
                    node.blocked.store(false);
                }
                break;
            }
            std::this_thread::yield();
        }
    }
    return isBlocaked;
}

bool ETSyncRoute::tryBlockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    bool isBlocked = false;
    {
        while(true) {
            bool tFalse = false;
            if(node.blocked.compare_exchange_weak(tFalse, true)) {
                if(isBlockable(node)) {
                    addThread(node, threadId);
                    isBlocked = true;
                } else {
                    node.blocked.store(false);
                }
                break;
            }
            std::this_thread::yield();
        }
    }
    return isBlocked;
}

void ETSyncRoute::unlockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        assert(node.blocked.load() && "Node isn't blocked");
        removeThread(node, threadId);
        node.blocked.store(false);
    }
}

bool ETSyncRoute::isUniqueThread(const Node& node, const std::thread::id& threadId) const {
    for(int i = 0; i < 4; ++i) {
        if(node.data[i].count > 0) {
            if(node.data[i].threadId != threadId) {
                return false;
            }
        }
    }
    return true;
}

void ETSyncRoute::removeThread(Node& node, const std::thread::id& threadId) {
    for(int i = 0; i < 4; ++i) {
        if(node.data[i].threadId == threadId) {
            assert(node.data[i].count > 0 && "Can't pop if there is no route");
            node.data[i].count -= 1;
            return;
        }
    }
    assert(false && "Can't find slot with thread");
}

void ETSyncRoute::addThread(Node& node, const std::thread::id& threadId) {
    for(int i = 0; i < 4; ++i) {
        auto& threadData = node.data[i];
        if(threadData.threadId == threadId) {
            threadData.count += 1;
            return;
        }
    }
    for(int i = 0; i < 4; ++i) {
        auto& threadData = node.data[i];
        if(threadData.count == 0) {
            threadData.threadId = threadId;
            threadData.count = 1;
            return;
        }
    }
    assert(false && "No free slot to add thread");
}

bool ETSyncRoute::isBlockable(const Node& node) const {
    for(int i = 0; i < 4; ++i) {
        if(node.data[i].count != 0) {
            return false;
        }
    }
    return true;
}

} // namespace ET