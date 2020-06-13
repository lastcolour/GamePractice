#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/TypeId.hpp"
#include "Core/Core.hpp"
#include "Core/ETSynchronization.hpp"

#include <type_traits>

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

    using ConnectionArrayT = std::vector<ETConnection>;
    using TypeConnectionMapT = std::unordered_map<TypeId, ConnectionArrayT>;

public:

    ETSystem();
    ~ETSystem();

    EntityId createNewEntityId() {
        EntityId resId;
        {
            std::lock_guard<std::mutex> lock(idGenMutex);
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
            auto etId = GetTypeId<ETType>();
            syncRoute.pushRoute(etId);
            auto it = activeConnection.find(etId);
            if(it != activeConnection.end()) {
                for(auto& currConn : it->second) {
                    resNodes.push_back(currConn.addressId);
                }
            }
            syncRoute.popRoute();
        }
        return resNodes;
    }

    template<typename ETType>
    bool isExistNode(EntityId addressId) const {
        bool res = false;
        if(!addressId.isValid()) {
            return res;
        }
        {
            auto etId = GetTypeId<ETType>();
            syncRoute.pushRoute(etId);
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
            syncRoute.popRoute();
        }
        return res;
    }

    template<template<class> class ETNode, class ETType>
    void connectNode(ETNode<ETType>& node, EntityId addressId) {
        if(!addressId.isValid()) {
            return;
        }
        ETConnectionRequest connReq;
        connReq.etId = GetTypeId<ETType>();
        connReq.isDisconnect = false;
        connReq.conn.node = &node;
        connReq.conn.addressId = addressId;
        handleConnectionRequest(connReq);
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>& node) {
        ETConnectionRequest connReq;
        connReq.etId = GetTypeId<ETType>();
        connReq.isDisconnect = true;
        connReq.conn.node = &node;
        connReq.conn.addressId = InvalidEntityId;
        handleConnectionRequest(connReq);
    }

    // ==--------------- Const ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId == addressId) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    (obj->*func)(std::forward<ParamType>(params)...);
                }
            }
        }

        syncRoute.popRoute(etId);
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId == addressId) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    retVal = (obj->*func)(std::forward<ParamType>(params)...);
                    break;
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId != InvalidEntityId) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    (obj->*func)(std::forward<ParamType>(params)...);
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId.isValid()) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    retVal = (obj->*func)(std::forward<ParamType>(params)...);
                    break;
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    // ==--------------- Regular ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId == addressId) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    (obj->*func)(std::forward<ParamType>(params)...);
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        if(!addressId.isValid()) {
            return;
        }

        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId == addressId) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    retVal = (obj->*func)(std::forward<ParamType>(params)...);
                    break;
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId.isValid()) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    (obj->*func)(std::forward<ParamType>(params)...);
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        ConnectionArrayT* connArr = nullptr;
        syncRoute.pushRoute(etId);

        if(isActiveConnectionExist(etId, connArr)) {
            for(auto& currConn : *connArr) {
                if(currConn.addressId.isValid()) {
                    auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                    retVal = (obj->*func)(std::forward<ParamType>(params)...);
                    break;
                }
            }
        }

        syncRoute.popRoute();
        updatePendingConnections();
    }

private:

    ETSystem(const ETSystem&) = delete;
    ETSystem& operator=(const ETSystem&) = delete;

private:

    void registerConnection(const ETConnectionRequest& connReq);
    void handleConnectionRequest(const ETConnectionRequest& connReq);
    void updatePendingConnections();
    bool isConnectRequestedDisconnect(const ETNodeBase& node) const;
    bool isDoubleConnect(TypeId etId, const ETNodeBase& node) const;
    bool isActiveConnectionExist(TypeId etId, ConnectionArrayT*& connectionArray);

private:

    TypeConnectionMapT activeConnection;
    std::vector<ETConnectionRequest> pendingConnection;
    mutable ETSyncRoute syncRoute;
    std::mutex idGenMutex;
    std::recursive_mutex pendingConnMutex;
    EntityId entityIdGen;
};

ETSystem* GetETSystem();

#endif /* __ET_SYSTEM_HPP__ */