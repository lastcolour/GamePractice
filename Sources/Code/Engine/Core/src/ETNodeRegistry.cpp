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

void ETNodeRegistry::connectNode(int etId, EntityId addressId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(!addressId.isValid()) {
        return;
    }
    auto isUnique = syncRoute->pushRoute(etId);
    if(isUnique) {
        doConnect(etId, addressId, ptr);
    } else {
        bool isConnect = true;
        connections[etId].pendingConnections.push_back({ptr, addressId, isConnect});
    }
    syncRoute->popRoute(etId);
}

void ETNodeRegistry::disconnectNode(int etId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    auto isUnique = syncRoute->pushRoute(etId);
    {
        if(isUnique) {
            doDisconnect(etId, ptr);
        } else {
            doSoftDisconnect(etId, ptr);
            bool isConnect = false;
            connections[etId].pendingConnections.push_back({ptr, InvalidEntityId, isConnect});
        }
    }
    syncRoute->popRoute(etId);
}

void ETNodeRegistry::forEachNode(int etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                callF(node.ptr);
            }
        }
    }
    endRoute(isUnique, etId);
}

void ETNodeRegistry::forEachNode(int etId, CallFunctionT callF) {
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                callF(node.ptr);
            }
        }
    }
    endRoute(isUnique, etId);
}

void ETNodeRegistry::forFirst(int etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                callF(node.ptr);
                break;
            }
        }
    }
    endRoute(isUnique, etId);
}

void ETNodeRegistry::forFirst(int etId, CallFunctionT callF) {
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                callF(node.ptr);
                break;
            }
        }
    }
    endRoute(isUnique, etId);
}

bool ETNodeRegistry::startRoute(int etId) {
    assert(etId < MAX_ET_NODE_TYPES && "Too many ET nodes types");
    return syncRoute->pushRoute(etId);
}

void ETNodeRegistry::endRoute(bool isUnique, int etId) {
    if(isUnique) {
        for(auto& req : connections[etId].pendingConnections) {
            if(req.isConnect) {
                doConnect(etId, req.id, req.ptr);
            } else {
                doDisconnect(etId, req.ptr);
            }
        }
        connections[etId].pendingConnections.clear();
    }
    syncRoute->popRoute(etId);
}

std::vector<EntityId> ETNodeRegistry::getAll(int etId) {
    std::vector<EntityId> result;
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id != InvalidEntityId) {
                result.push_back(node.id);
            }
        }
    }
    endRoute(isUnique, etId);
    return result;
}

bool ETNodeRegistry::isExist(int etId, EntityId addressId) {
    if(!addressId.isValid()) {
        return false;
    }
    bool result = false;
    auto isUnique = startRoute(etId);
    {
        for(auto& node : connections[etId].nodes) {
            if(node.id == addressId) {
                result = true;
                break;
            }
        }
    }
    endRoute(isUnique, etId);
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
    }
    auto isUnique = startRoute(etId);
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
    endRoute(isUnique, etId);
}