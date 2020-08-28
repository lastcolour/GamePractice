#include "ETSynchronization.hpp"

#include <cassert>
#include <algorithm>
#include <cstdio>

ETSyncRoute::ETSyncRoute(int etNodesCount) :
    blockedRouteMap(etNodesCount) {
}

ETSyncRoute::~ETSyncRoute() {
}

void ETSyncRoute::softBlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::unique_lock<std::mutex> ulock(mutex);
        if(isUniqueThread(node, threadId)) {
            assert(!node.blocked && "Node already blocked");
            node.blocked = true;
        } else {
            cond.wait(ulock, [this, &node, &threadId](){
                return isUniqueThread(node, threadId);
            });
            assert(!node.blocked && "Node already blocked");
            node.blocked = true;
        }
        printf("(%d) [ETSyncRoute] softBlock()\n", getThreadId(node, threadId));
    }
}

void ETSyncRoute::softUnlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        printf("(%d) [ETSyncRoute] softUnlock()\n", getThreadId(node, threadId));
        assert(isUniqueThread(node, threadId) && "This thread isn't unique");
        assert(node.blocked && "Node isn't blocked");
        node.blocked = false;
    }
}

void ETSyncRoute::pushRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::unique_lock<std::mutex> ulock(mutex);
        if(node.blocked) {
            cond.wait(ulock, [this, &node](){
                return !node.blocked;
            });
        }
        assert(!node.blocked && "Node is blocked");
        addThread(node, threadId);
        printf("(%d) [ETSyncRoute] pushRoute()\n", getThreadId(node, threadId));
    }
   
}

void ETSyncRoute::popRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        printf("(%d) [ETSyncRoute] popRoute()\n", getThreadId(node, threadId));
        removeThread(node, threadId);
        cond.notify_all();
    }
}

bool ETSyncRoute::blockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(!node.blocked && isBlockable(node)) {
            addThread(node, threadId);
            node.blocked = true;
            printf("(%d) [ETSyncRoute] blockRoute() -- SUCCESS\n", getThreadId(node, threadId));
            return true;
        } else {
            printf("(%d) [ETSyncRoute] blockRoute() -- FAIL\n", getThreadId(node, threadId));
        }
    }
    return false;
}

void ETSyncRoute::unlockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        printf("(%d) [ETSyncRoute] unlockRoute()\n", getThreadId(node, threadId));
        assert(node.blocked && "Node isn't blocked");
        removeThread(node, threadId);
        node.blocked = false;
        cond.notify_all();
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
        if(node.data[i].threadId == threadId) {
            node.data[i].count += 1;
            return;
        }
    }
    for(int i = 0; i < 4; ++i) {
        if(node.data[i].threadId == std::thread::id()) {
            node.data[i].threadId = threadId;
            node.data[i].count = 1;
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

int ETSyncRoute::getThreadId(const Node& node, const std::thread::id& threadId) const {
    for(int i = 0; i < 4; ++i) {
        if(node.data[i].threadId == threadId) {
            return i;
        }
    }
    return -1;
}