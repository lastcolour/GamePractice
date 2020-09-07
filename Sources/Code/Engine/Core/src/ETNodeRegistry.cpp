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
    {
        auto& ptrToIdMap = connections[etId].ptrToIdMap;
        if(ptrToIdMap.count(ptr)) {
            return;
        }
        ptrToIdMap[ptr] = addressId;
    }
    {
        auto& idToPtrMap = connections[etId].idToPtrMap;
        idToPtrMap.insert({addressId, ptr});
    }
}

void ETNodeRegistry::doDisconnect(int etId, ETNodeBase* ptr) {
    EntityId ptrId;
    {
        auto& ptrToIdMap = connections[etId].ptrToIdMap;
        auto it = ptrToIdMap.find(ptr);
        if(it == ptrToIdMap.end()) {
            return;
        }
        ptrId = it->second;
        ptrToIdMap.erase(it);
    }
    {
        auto& idToPtrMap = connections[etId].idToPtrMap;
        auto range = idToPtrMap.equal_range(ptrId);
        auto removeIt = idToPtrMap.end();
        for(auto& it = range.first; it != range.second; ++it) {
            if(it->second == ptr || it->second == nullptr) {
                removeIt = it;
                break;
            }
        }
        idToPtrMap.erase(removeIt);
    }
}

void ETNodeRegistry::doSoftDisconnect(int etId, ETNodeBase* ptr) {
    EntityId ptrId;
    {
        auto& ptrToIdMap = connections[etId].ptrToIdMap;
        auto it = ptrToIdMap.find(ptr);
        if(it == ptrToIdMap.end()) {
            return;
        }
        ptrId = it->second;
    }
    {
        auto& idToPtrMap = connections[etId].idToPtrMap;
        auto range = idToPtrMap.equal_range(ptrId);
        auto removeIt = idToPtrMap.end();
        for(auto& it = range.first; it != range.second; ++it) {
            if(it->second == ptr) {
                it->second = nullptr;
                break;
            }
        }
    }
}

void ETNodeRegistry::connectNode(int etId, EntityId addressId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(!addressId.isValid()) {
        return;
    }
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
        auto range = connections[etId].idToPtrMap.equal_range(addressId);
        for(auto it = range.first; it != range.second; ++it) {
            if(it->second != nullptr) {
                callF(it->second);
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::forEachNode(int etId, CallFunctionT callF) {
    startRoute(etId);
    {
        for(auto& node : connections[etId].idToPtrMap) {
            if(node.second != nullptr) {
                callF(node.second);
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
        auto range = connections[etId].idToPtrMap.equal_range(addressId);
        for(auto it = range.first; it != range.second; ++it) {
            if(it->second != nullptr) {
                callF(it->second);
                break;
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::forFirst(int etId, CallFunctionT callF) {
    startRoute(etId);
    {
        for(auto& node : connections[etId].idToPtrMap) {
            if(node.second != nullptr) {
                callF(node.second);
                break;
            }
        }
    }
    endRoute(etId);
}

void ETNodeRegistry::startRoute(int etId) {
    assert(etId < MAX_ET_NODE_TYPES && "Too many ET nodes types");
    syncRoute->pushRoute(etId);
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
        for(auto& node : connections[etId].idToPtrMap) {
            if(node.second != nullptr) {
                result.push_back(node.first);
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
        auto range = connections[etId].idToPtrMap.equal_range(addressId);
        for(auto it = range.first; it != range.second; ++it) {
            if(it->second != nullptr) {
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
        auto& nodes = connections[etId].idToPtrMap;
        for(auto& event : events) {
            if(event.id == InvalidEntityId) {
                for(auto& node : nodes) {
                    if(node.second != nullptr) {
                        event.callF(node.second);
                    }
                }
            } else {
                auto range = connections[etId].idToPtrMap.equal_range(event.id);
                for(auto it = range.first; it != range.second; ++it) {
                    if(it->second != nullptr) {
                        event.callF(it->second);
                    }
                }
            }
        }
    }
    endRoute(etId);
}