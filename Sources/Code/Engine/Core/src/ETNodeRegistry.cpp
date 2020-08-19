#include "Core/ETNodeRegistry.hpp"
#include "ETSynchronization.hpp"

#include <cassert>

ETNodeRegistry::ETNodeRegistry() :
    syncRoute(new ETSyncRoute()) {
}

ETNodeRegistry::~ETNodeRegistry() {
}

void ETNodeRegistry::doConnect(TypeId etId, EntityId addressId, ETNodeBase* ptr) {
    NodesT* nodes = nullptr;
    auto it = connections.find(etId);
    if(it == connections.end()) {
        nodes = &(connections[etId]);
    } else {
        nodes = &it->second;
    }
    auto nodeIt = std::find_if(nodes->begin(), nodes->end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(nodeIt != nodes->end()) {
        return;
    }
    nodes->push_back({ptr, addressId});
}

void ETNodeRegistry::doDisconnect(TypeId etId, ETNodeBase* ptr) {
    auto it = connections.find(etId);
    if(it == connections.end()) {
        return;
    }
    auto& nodes = it->second;
    auto nodeIt = std::find_if(nodes.begin(), nodes.end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(nodeIt == nodes.end()) {
        return;
    }
    *nodeIt = std::move(nodes.back());
    nodes.pop_back();
}

void ETNodeRegistry::doSoftDisconnect(TypeId etId, ETNodeBase* ptr) {
    auto it = connections.find(etId);
    if(it == connections.end()) {
        return;
    }
    auto& nodes = it->second;
    auto nodeIt = std::find_if(nodes.begin(), nodes.end(), [ptr](const Node& node){
        return node.ptr == ptr;
    });
    if(nodeIt == nodes.end()) {
        return;
    }
    nodeIt->id = InvalidEntityId;
}

void ETNodeRegistry::connectNode(TypeId etId, EntityId addressId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(!addressId.isValid()) {
        return;
    }
    if(syncRoute->tryPushUniqueRoute(etId)) {
        doConnect(etId, addressId, ptr);
        syncRoute->popRoute();
    } else {
        std::lock_guard<std::recursive_mutex> lock(pendingConnMutex);
        bool isConnect = true;
        pendingConnections.push_back({ptr, addressId, etId, isConnect});
    }
}

void ETNodeRegistry::disconnectNode(TypeId etId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    syncRoute->pushRoute(etId);
    {
        if(syncRoute->isRouteUniqueForCurrentThread(etId)) {
            doDisconnect(etId, ptr);
        } else {
            doSoftDisconnect(etId, ptr);
            std::lock_guard<std::recursive_mutex> lock(pendingConnMutex);
            bool isConnect = false;
            pendingConnections.push_back({ptr, InvalidEntityId, etId, isConnect});
        }
    }
    syncRoute->popRoute();
}

void ETNodeRegistry::forEachNode(TypeId etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id == addressId) {
                    callF(node.ptr);
                }
            }
        }
    }
    endRoute();
}

void ETNodeRegistry::forEachNode(TypeId etId, CallFunctionT callF) {
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id != InvalidEntityId) {
                    callF(node.ptr);
                }
            }
        }
    }
    endRoute();
}

void ETNodeRegistry::forFirst(TypeId etId, EntityId addressId, CallFunctionT callF) {
    if(!addressId.isValid()) {
        return;
    }
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id == addressId) {
                    callF(node.ptr);
                    break;
                }
            }
        }
    }
    endRoute();
}

void ETNodeRegistry::forFirst(TypeId etId, CallFunctionT callF) {
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id != InvalidEntityId) {
                    callF(node.ptr);
                    break;
                }
            }
        }
    }
    endRoute();
}

void ETNodeRegistry::startRoute(TypeId etId) {
    syncRoute->pushRoute(etId);
}

void ETNodeRegistry::endRoute() {
    syncRoute->popRoute();
    std::lock_guard<std::recursive_mutex> lock(pendingConnMutex);
    auto it = pendingConnections.begin();
    while(it != pendingConnections.end()) {
        auto& pendConn = *it;
        if(syncRoute->tryPushUniqueRoute(pendConn.etId)) {
            if(pendConn.isConnect) {
                doConnect(pendConn.etId, pendConn.id, pendConn.ptr);
            } else {
                doDisconnect(pendConn.etId, pendConn.ptr);
            }
            syncRoute->popRoute();
            it = pendingConnections.erase(it);
        } else {
            ++it;
        }
    }
}

std::vector<EntityId> ETNodeRegistry::getAll(TypeId etId) {
    std::vector<EntityId> result;
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id != InvalidEntityId) {
                    result.push_back(node.id);
                }
            }
        }
    }
    endRoute();
    return result;
}

bool ETNodeRegistry::isExist(TypeId etId, EntityId addressId) {
    if(!addressId.isValid()) {
        return false;
    }
    bool result = false;
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            for(auto& node : it->second) {
                if(node.id == addressId) {
                    result = true;
                    break;
                }
            }
        }
    }
    endRoute();
    return result;
}

void ETNodeRegistry::queueEventForAddress(TypeId etId, EntityId addressId, CallFunctionT callF) {
    std::lock_guard<std::mutex> lock(eventMutex);
    auto it = pendingEvents.find(etId);
    if(it != pendingEvents.end()) {
        it->second.emplace_back(Event{callF, addressId});
    } else {
        pendingEvents.emplace(etId, std::vector<Event>{Event{callF, addressId}});
    }
}

void ETNodeRegistry::queueEventForAll(TypeId etId, CallFunctionT callF) {
    std::lock_guard<std::mutex> lock(eventMutex);
    auto it = pendingEvents.find(etId);
    if(it != pendingEvents.end()) {
        it->second.emplace_back(Event{callF, InvalidEntityId});
    } else {
        pendingEvents.emplace(etId, std::vector<Event>{Event{callF, InvalidEntityId}});
    }
}

void ETNodeRegistry::pollEventsForAll(TypeId etId) {
    std::vector<Event> events;
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        auto it = pendingEvents.find(etId);
        if(it == pendingEvents.end()) {
            return;
        }
        if(it->second.empty()) {
            return;
        }
        events = std::move(it->second);
    }
    startRoute(etId);
    {
        auto it = connections.find(etId);
        if(it != connections.end()) {
            auto& nodes = it->second;
            for(auto& event : events) {
                for(auto& node : nodes) {
                    if((event.id == InvalidEntityId || event.id == node.id) && node.id != InvalidEntityId) {
                        event.callF(node.ptr);
                    }
                }
            }
        }
    }
    endRoute();
}