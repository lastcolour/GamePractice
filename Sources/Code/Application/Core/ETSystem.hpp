#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/TypeId.hpp"
#include "Core/Core.hpp"

#include <unordered_map>
#include <type_traits>
#include <vector>
#include <mutex>
#include <thread>

class ETNodeBase;

template<typename T>
class ETNode;

class ETSystem {
private:

    struct ETConnection {
        ETNodeBase* node;
        EntityId addressId;
    };

    struct ETConnectionRequest {
        ETConnection conn;
        TypeId etId;
        bool isDisconnect;
    };

private:

    using MutexTypeT = std::recursive_mutex;
    using LockTypeT = std::lock_guard<MutexTypeT>;

    using ConnectionArrayT = std::vector<ETConnection>;
    using TypeConnectionMapT = std::unordered_map<TypeId, ConnectionArrayT>;

public:

    ETSystem();
    ~ETSystem();

    EntityId createNewEntityId() {
        EntityId resId;
        {
            LockTypeT lock(mutex);

            auto endId = entityIdGen.getRawId();
            entityIdGen.setRawId(++endId);
            resId = entityIdGen;
        }
        return resId;
    }

    template<typename ETType>
    std::vector<EntityId> getAll() const {
        std::vector<EntityId> resNodes;
        {
            LockTypeT lock(mutex);

            auto etId = GetTypeId<ETType>();
            auto it = activeConnection.find(etId);
            if(it != activeConnection.end()) {
                for(auto& currConn : it->second) {
                    resNodes.push_back(currConn.addressId);
                }
            }
        }
        return resNodes;
    }

    template<typename ETType>
    bool isExistNode(EntityId addressId) const {
        bool res = false;
        if (addressId == InvalidEntityId) {
            return res;
        }
        {
            LockTypeT lock(mutex);

            auto etId = GetTypeId<ETType>();
            auto it = activeConnection.find(etId);
            if(it != activeConnection.end()) {
                auto& etConnection = it->second;
                for(auto etIt = etConnection.begin(), etEnd = etConnection.end(); etIt != etEnd; ++etIt) {
                    if(etIt->addressId == addressId) {
                        res = true;
                        break;
                    }
                }
            }
        }
        return res;
    }

    template<template<class> class ETNode, class ETType>
    void connectNode(ETNode<ETType>& node, EntityId addressId) {
        if (addressId == InvalidEntityId) {
            return;
        }
        LockTypeT lock(mutex);

        ETConnectionRequest connReq;
        connReq.etId = GetTypeId<ETType>();
        connReq.isDisconnect = false;
        connReq.conn.node = &node;
        connReq.conn.addressId = addressId;

        if(isConnectRequestedDisconnect(node)) {
            pendingConnection.push_back(connReq);
        } else if (isDoubleConnect(connReq.etId, node)) {
            return;
        } else {
            if(isRouteSafe(connReq.etId)) {
                registerConnection(connReq);
            } else {
                pendingConnection.push_back(connReq);
            }
        }
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>& node) {
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it != activeConnection.end()) {
            auto& connections = it->second;
            for(auto& currConn: connections) {
                if(currConn.node == &node) {
                    currConn.addressId = InvalidEntityId;
                }
            }
        }

        ETConnectionRequest connReq;
        connReq.etId = etId;
        connReq.isDisconnect = true;
        connReq.conn.node = &node;
        connReq.conn.addressId = InvalidEntityId;

        if(isRouteSafe(connReq.etId)) {
            registerConnection(connReq);
        } else {
            pendingConnection.push_back(connReq);
        }
    }

    // ==--------------- Const ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                break;
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId != InvalidEntityId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId != InvalidEntityId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                break;
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    // ==--------------- Regular ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                break;
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if (currConn.addressId != InvalidEntityId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        LockTypeT lock(mutex);
    
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        activeRoute.push_back(etId);
        for(auto& currConn : it->second) {
            if(currConn.addressId != InvalidEntityId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                break;
            }
        }
        activeRoute.pop_back();
        updatePendingConnections();
    }

private:

    ETSystem(const ETSystem&) = delete;
    ETSystem& operator=(const ETSystem&) = delete;

private:

    void registerConnection(const ETConnectionRequest& connReq);
    void updatePendingConnections();
    bool isRouteSafe(TypeId etId) const;
    bool isConnectRequestedDisconnect(const ETNodeBase& node) const;
    bool isDoubleConnect(TypeId etId, const ETNodeBase& node) const;

private:

    TypeConnectionMapT activeConnection;
    mutable MutexTypeT mutex;
    EntityId entityIdGen;
    std::vector<ETConnectionRequest> pendingConnection;
    std::vector<TypeId> activeRoute;
};

ETSystem* GetETSystem();

#endif /* __ET_SYSTEM_HPP__ */