#include "Core/ETNodeRegistry.hpp"
#include "ETSynchronization.hpp"
#include "Core/ETSystem.hpp"

#include <cassert>

namespace {

const int MAX_ET_NODE_TYPES = 128;

} // namespace

ETNodeRegistry::ETNodeRegistry() :
    eventAllocator(ET::MAX_EVENT_SIZE, 2048),
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
    if(syncRoute->tryHardBlock(etId)) {
        doConnect(etId, addressId, ptr);
    } else {
        std::lock_guard<std::mutex> lock(connMutex);
        bool isConnect = true;
        connections[etId].pendingConnections.push_back({ptr, addressId, isConnect});
    }
    syncRoute->hardUnlock(etId);
}

void ETNodeRegistry::disconnectNode(int etId, ETNodeBase* ptr) {
    assert(ptr && "Invalid ptr");
    if(syncRoute->tryHardBlock(etId)) {
        doDisconnect(etId, ptr);
    } else {
        doSoftDisconnect(etId, ptr);
        bool isConnect = false;
        {
            std::lock_guard<std::mutex> lock(connMutex);
            connections[etId].pendingConnections.push_back({ptr, InvalidEntityId, isConnect});
        }
    }
    syncRoute->hardUnlock(etId);
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

void ETNodeRegistry::queueEvent(int etId, ET::ETDefferedCallBase* defferedCall) {
    if(!defferedCall) {
        assert(false && "Invalid deffered event");
        return;
    }
    std::lock_guard<std::mutex> lock(eventMutex);
    connections[etId].pendingEvents.emplace_back(defferedCall);
}

void ETNodeRegistry::pollEventsForAll(int etId) {
    auto& events = connections[etId].processEvents;
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        if(connections[etId].pendingEvents.empty()) {
            return;
        }
        std::swap(events, connections[etId].pendingEvents);
        connections[etId].pendingEvents.clear();
    }
    startRoute(etId);
    {
        auto& nodes = connections[etId].idToPtrMap;
        for(auto& event : events) {
            if(event->addressId == InvalidEntityId) {
                for(auto& node : nodes) {
                    if(node.second != nullptr) {
                        event->call(node.second);
                    }
                }
            } else {
                auto range = connections[etId].idToPtrMap.equal_range(event->addressId);
                for(auto it = range.first; it != range.second; ++it) {
                    if(it->second != nullptr) {
                        event->call(it->second);
                    }
                }
            }
        }
    }
    endRoute(etId);
    {
        std::lock_guard<std::mutex> lock(eventMutex);
        for(auto& event : events) {
            event->~ETDefferedCallBase();
            eventAllocator.deallocate(event);
        }
        events.clear();
    }
}