#include "ETSynchronization.hpp"

#include <cassert>
#include <algorithm>

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
            mutex.lock();
            if(!node.blocked && isUniqueThread(node, threadId)) {
                node.blocked = true;
                break;
            } else {
                mutex.unlock();
                std::this_thread::yield();
            }
        }
        isBlocked = isBlockable(node);
        mutex.unlock();
    }
    return isBlocked;
}

void ETSyncRoute::hardUnlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        assert(isUniqueThread(node, threadId) && "This thread isn't unique");
        assert(node.blocked && "Node isn't blocked");
        node.blocked = false;
    }
}

void ETSyncRoute::pushRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        while(true) {
            mutex.lock();
            if(!node.blocked) {
                break;
            } else {
                mutex.unlock();
                std::this_thread::yield();
            }
        }
        addThread(node, threadId);
        mutex.unlock();
    }
}

void ETSyncRoute::popRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        removeThread(node, threadId);
    }
}

bool ETSyncRoute::popAndBlock(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        removeThread(node, threadId);
        if(!node.blocked && isBlockable(node)) {
            addThread(node, threadId);
            node.blocked = true;
            return true;
        }
    }
    return false;
}

bool ETSyncRoute::tryBlockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        if(!node.blocked && isBlockable(node)) {
            addThread(node, threadId);
            node.blocked = true;
            return true;
        }
    }
    return false;
}

void ETSyncRoute::unlockRoute(int reqRouteId) {
    auto& node = blockedRouteMap[reqRouteId];
    auto threadId = std::this_thread::get_id();
    {
        std::lock_guard<std::mutex> lock(mutex);
        assert(node.blocked && "Node isn't blocked");
        removeThread(node, threadId);
        node.blocked = false;
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