#ifndef __ET_NODE_REGISTRY_HPP__
#define __ET_NODE_REGISTRY_HPP__

#include "Core/MainMemoryAllocator.hpp"
#include "Core/GlobalEnvironment.hpp"
#include "Core/ETIterator.hpp"

namespace ET {

class ETSyncRoute;
class ETSystem;

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

    std::vector<EntityId> getAll(int etId);
    bool isExist(int etId, EntityId addressId);
    bool isExist(int etId);

    void queueEvent(int etId, ETDefferedCallBase* defferedCall);
    void pollEventsForAll(int etId);

    ETIterator createIterator(int etId) {
        startRoute(etId);
        auto& container = connections[etId].idToPtrMap;
        return ETIterator(container.begin(), container.end(), etId);
    }

    ETIterator createIterator(int etId, EntityId addressId) {
        startRoute(etId);
        auto range = connections[etId].idToPtrMap.equal_range(addressId);
        return ETIterator(range.first, range.second, etId);
    }

    void destroyIterator(const ETIterator& it) {
        endRoute(it.getETId());
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

    std::unique_ptr<ETSyncRoute> syncRoute;
    std::vector<Registry> connections;
    std::mutex connMutex;
    std::mutex eventMutex;
};

} // namespace ET

#endif /* __ET_NODE_REGISTRY_HPP__ */