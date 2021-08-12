#ifndef __ET_NODE_REGISTRY_HPP__
#define __ET_NODE_REGISTRY_HPP__

#include "Core/Core.hpp"
#include "Core/PoolAllocator.hpp"

#include <mutex>

class ETNodeBase;

namespace ET {

class ETSyncRoute;

class ETNodeRegistry {
public:

    struct Node {
        ETNodeBase* ptr;
        EntityId id;
    };

    struct ConnectionRequest {
        ETNodeBase* ptr;
        EntityId id;
        bool isConnect;
    };

    using NodesT = std::vector<Node>;

public:

    ETNodeRegistry();
    ~ETNodeRegistry();

    void connectNode(int etId, EntityId addressId, ETNodeBase* ptr);
    void disconnectNode(int etId, ETNodeBase* ptr);

    template<typename FuncT>
    void forEachNode(int etId, EntityId addressId, FuncT&& callF) {
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

    template<typename FuncT>
    void forEachNode(int etId, FuncT&& callF) {
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

    template<typename FuncT>
    void forFirst(int etId, EntityId addressId, FuncT&& callF) {
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

    template<typename FuncT>
    void forFirst(int etId, FuncT&& callF) {
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

    std::vector<EntityId> getAll(int etId);
    bool isExist(int etId, EntityId addressId);

    void queueEvent(int etId, ETDefferedCallBase* defferedCall);
    void pollEventsForAll(int etId);

    template<typename T>
    void* allocDefferedEvent() {
        static_assert(sizeof(T) <= MAX_EVENT_SIZE, "Event size is too big");
        std::lock_guard<std::mutex> lock(eventMutex);
        return eventAllocator.allocate();
    }

private:

    void doSoftDisconnect(int etId, ETNodeBase* ptr);
    void doConnect(int etId, EntityId addressId, ETNodeBase* ptr);
    void doDisconnect(int etId, ETNodeBase* ptr);
    void startRoute(int etId);
    void endRoute(int etId);
    void processPendingConnections(int etId);

private:

    struct Registry {
        std::unordered_map<ETNodeBase*, EntityId> ptrToIdMap;
        std::unordered_multimap<EntityId, ETNodeBase*> idToPtrMap;
        std::vector<ET::ETDefferedCallBase*> pendingEvents;
        std::vector<ET::ETDefferedCallBase*> processEvents;
        std::vector<ConnectionRequest> pendingConnections;
    };

private:

    PoolAllocator eventAllocator;
    std::unique_ptr<ETSyncRoute> syncRoute;
    std::vector<Registry> connections;
    std::mutex connMutex;
    std::mutex eventMutex;
};

} // namespace ET

#endif /* __ET_NODE_REGISTRY_HPP__ */