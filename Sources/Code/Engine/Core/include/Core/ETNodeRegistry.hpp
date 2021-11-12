#ifndef __ET_NODE_REGISTRY_HPP__
#define __ET_NODE_REGISTRY_HPP__

#include "Core/MainMemoryAllocator.hpp"
#include "Core/ETUtils.hpp"

namespace ET {

class ETSyncRoute;
class ETSystem;

class ETNodeBase {
protected:
    virtual ~ETNodeBase() = default;
};

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
    bool isExist(int etId);

    void queueEvent(int etId, ETDefferedCallBase* defferedCall);
    void pollEventsForAll(int etId);

    template<typename T, typename ... ArgsType>
    T* createDefferedEvent(ArgsType&& ... args) {
        auto ptr = GetEnv()->GetMemoryAllocator()->allocate(sizeof(T));
        return new (ptr) T(std::forward<ArgsType>(args)...);
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

template<typename T>
class ETNode : public ET::ETNodeBase, public T {

    friend class ET::ETSystem;

public:

    ETNode() {
        static_assert(std::is_abstract<T>::value, "ETType can be only abstract class");
    }
    virtual ~ETNode() {
        disconnect();
    }

protected:

    void connect(EntityId adId) {
        GetEnv()->GetETSystem()->connectNode(*this, adId);
    }
    void disconnect() {
        GetEnv()->GetETSystem()->disconnectNode(*this);
    }

private:

    ETNode(const ETNode&) = delete;
    ETNode& operator=(const ETNode&) = delete;
};

#endif /* __ET_NODE_REGISTRY_HPP__ */