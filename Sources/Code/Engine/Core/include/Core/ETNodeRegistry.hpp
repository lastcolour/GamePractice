#ifndef __ET_NODE_REGISTRY_HPP__
#define __ET_NODE_REGISTRY_HPP__

#include "Core/Core.hpp"

#include <vector>
#include <mutex>
#include <memory>
#include <unordered_map>

class ETNodeBase;
class ETSyncRoute;

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
        bool isConnect;
    };

    using NodesT = std::vector<Node>;

public:

    ETNodeRegistry();
    ~ETNodeRegistry();

    void connectNode(int etId, EntityId addressId, ETNodeBase* ptr);
    void disconnectNode(int etId, ETNodeBase* ptr);

    void forEachNode(int etId, EntityId addressId, CallFunctionT callF);
    void forEachNode(int etId, CallFunctionT callF);

    void forFirst(int etId, EntityId addressId, CallFunctionT callF);
    void forFirst(int etId, CallFunctionT callF);

    std::vector<EntityId> getAll(int etId);
    bool isExist(int etId, EntityId addressId);

    void queueEventForAddress(int etId, EntityId addressId, CallFunctionT callF);
    void queueEventForAll(int etId, CallFunctionT callF);
    void pollEventsForAll(int etId);

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
        std::vector<Event> pendingEvents;
        std::vector<ConnectionRequest> pendingConnections;
    };

private:

    std::unique_ptr<ETSyncRoute> syncRoute;
    std::vector<Registry> connections;
    std::mutex connMutex;
    std::mutex eventMutex;
};

#endif /* __ET_NODE_REGISTRY_HPP__ */