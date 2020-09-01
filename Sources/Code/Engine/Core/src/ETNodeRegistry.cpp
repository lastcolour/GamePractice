#include "Core/ETNodeRegistry.hpp"
#include "ETSynchronization.hpp"
#include "Core/ETSystem.hpp"

#include <cassert>

namespace {

const int MAX_ET_NODE_TYPES = 128;

} // namespace

ETNodeRegistry::ETNodeRegistry() :
    syncRoute(new ETSyncRoute(MAX_ET_NODE_TYPES)),
    connections(MAX_ET_NODE_TYPES) {
}

ETNodeRegistry::~ETNodeRegistry() {
}

void ETNodeRegistry::doConnect(int etId, EntityId addressId, ETNodeBase* ptr) {
    auto& nodes = connections[etId].nodes;
    auto nodeIt = std::find_if(nodes.begin(), nodes.end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(nodeIt != nodes.end()) {
        return;
    }
    nodes.push_back({ptr, addressId});
}

void ETNodeRegistry::doDisconnect(int etId, ETNodeBase* ptr) {
    auto& nodes = connections[etId].nodes;
    auto it = std::find_if(nodes.begin(), nodes.end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(it == nodes.end()) {
        return;
    }
    *it = std::move(nodes.back());
    nodes.pop_back();
}

void ETNodeRegistry::doSoftDisconnect(int etId, ETNodeBase* ptr) {
    auto& nodes = connections[etId].nodes;
    auto it = std::find_if(nodes.begin(), nodes.end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(it == nodes.end()) {
        return;
    }
    it->id = InvalidEntityId;
}

void ETNodeRegistry::connectNode(int etId, bool isThreadSafe, EntityId addressId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(!addressId.isValid()) {
        return;
    }
    connections[etId].isThreadSafe = isThreadSafe;
    if(syncRoute->tryBlockRoute(etId)) {
        doConnect(etId, addressId, ptr);
        processPendingConnections(etId);
        syncRoute->unlockRoute(etId);
    } else {
        std::lock_guard<std::mutex> lock(connMutex);
        bool isConnect = true;
        connections[etId].pendingConnections.push_back({ptr, addressId, isConnect});
    }
}

void ETNodeRegistry::disconnectNode(int etId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(syncRoute->tryHardBlock(etId)) {
        doDisconnect(etId, ptr);
        processPendingConnections(etId);
    } else {
        doSoftDisconnect(etId, ptr);
        bool isConnect = false;
        connections[etId].pendingConnections.push_back({ptr, InvalidEntityId, isConnect});
    }
    syncRoute->hardUnlock(etId);
}

void ETNodeRegistry::forEachNode(int etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                callF(node.ptr);
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::forEachNode(int etId, CallFunctionT callF) {
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                callF(node.ptr);
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::forFirst(int etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                callF(node.ptr);
                break;
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::forFirst(int etId, CallFunctionT callF) {
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                callF(node.ptr);
                break;
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::startRoute(int etId) {
    assert(etId < MAX_ET_NODE_TYPES && "Too many ET nodes types");
    bool isThreadIntersection = syncRoute->pushRoute(etId);
    if(!connections[etId].isThreadSafe) {
        assert(isThreadIntersection && "Thread intersection for non-thread safe routes");
    }
}

void ETNodeRegistry::endRoute(int etId) {
    if(syncRoute->popAndBlock(etId)) {
        processPendingConnections(etId);
        syncRoute->unlockRoute(etId);
    }
}

void ETNodeRegistry::processPendingConnections(int etId) {
    std::vector<ConnectionRequest> connectionReq;
    {
        std::lock_guard<std::mutex> lock(connMutex);
        connectionReq = std::move(connections[etId].pendingConnections);
    }
    for(auto& req : connectionReq) {
        if(req.isConnect) {
            doConnect(etId, req.id, req.ptr);
        } else {
            doDisconnect(etId, req.ptr);
        }
    }
}

std::vector<EntityId> ETNodeRegistry::getAll(int etId) {
    std::vector<EntityId> result;
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                result.push_back(node.id);
            }
        }
    }
    endRoute(etId);
    return result;
}

bool ETNodeRegistry::isExist(int etId, EntityId addressId) {
    if(!addressId.isValid()) {
        return false;
    }
    bool result = false;
    startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                result = true;
                break;
            }
        }
    }
    endRoute(etId);
    return result;
}

void ETNodeRegistry::queueEventForAddress(int etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    std::lock_guard<std::mutex> lock(eventMutex);
    connections[etId].pendingEvents.emplace_back(Event{callF, addressId});
}

void ETNodeRegistry::queueEventForAll(int etId, CallFunctionT callF) {
    std::lock_guard<std::mutex> lock(eventMutex);
    connections[etId].pendingEvents.emplace_back(Event{callF, InvalidEntityId});
}

void ETNodeRegistry::pollEventsForAll(int etId) {
    std::vector<Event> events;
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        events = std::move(connections[etId].pendingEvents);
        if(events.empty()) {
            return;
        }
    }
    startRoute(etId);
    {
        auto& nodes = connections[etId].nodes;
        for(auto& event : events) {
            for(auto& node : nodes) {
                if((event.id == InvalidEntityId || event.id == node.id) && node.id != InvalidEntityId) {
                    event.callF(node.ptr);
                }
            }
        }
    }
    endRoute(etId);
}