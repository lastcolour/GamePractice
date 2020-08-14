#ifndef __ET_NODE_REGISTRY_HPP__
#define __ET_NODE_REGISTRY_HPP__

#include "Core/TypeId.hpp"
#include "Core/ETSynchronization.hpp"

#include <functional>
#include <vector>
#include <unordered_map>

class ETNodeBase;

class ETNodeRegistry {
public:

    struct Node {
        ETNodeBase* ptr;
        EntityId id;
    };

    using CallFunctionT = std::function<void(ETNodeBase*)>;

    struct Event {
        CallFunctionT callF;
        EntityId id;
    };

    struct ConnectionRequest {
        ETNodeBase* ptr;
        EntityId id;
        TypeId etId;
        bool isConnect;
    };

    using NodesT = std::vector<Node>;

public:

    ETNodeRegistry();
    ~ETNodeRegistry();

    void connectNode(TypeId etId, EntityId addressId, ETNodeBase* ptr);
    void disconnectNode(TypeId etId, ETNodeBase* ptr);

    void forEachNode(TypeId etId, EntityId addressId, CallFunctionT callF);
    void forEachNode(TypeId etId, CallFunctionT callF);

    void forFirst(TypeId etId, EntityId addressId, CallFunctionT callF);
    void forFirst(TypeId etId, CallFunctionT callF);

    std::vector<EntityId> getAll(TypeId etId);
    bool isExist(TypeId etId, EntityId addressId);

    void queueEventForAddress(TypeId etId, EntityId addressId, CallFunctionT callF);
    void queueEventForAll(TypeId etId, CallFunctionT callF);
    void pollAllEvents(TypeId etId);

private:

    void doSoftDisconnect(TypeId etId, ETNodeBase* ptr);
    void doConnect(TypeId etId, EntityId addressId, ETNodeBase* ptr);
    void doDisconnect(TypeId etId, ETNodeBase* ptr);
    void startRoute(TypeId etId);
    void endRoute();

private:

    ETSyncRoute syncRoute;
    std::recursive_mutex pendingConnMutex;
    std::mutex eventMutex;
    std::unordered_map<TypeId, NodesT> connections;
    std::vector<ConnectionRequest> pendingConnections;
    std::unordered_map<TypeId, std::vector<Event>> pendingEvents;
};

#endif /* __ET_NODE_REGISTRY_HPP__ */