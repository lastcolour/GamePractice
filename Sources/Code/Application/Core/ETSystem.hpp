#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/TypeId.hpp"
#include "Core/Core.hpp"

#include <unordered_map>
#include <vector>

class ETNodeBase;

template<typename T>
class ETNode;

class ETSystem {
private:

    struct ETConnection {
        ETNodeBase* node;
        EntityId addressId;
    };

public:

    ETSystem() :
        entityIdGen(InvalidEntityId) {}
    ~ETSystem() = default;

    EntityId createNewEntityId() {
        ++entityIdGen;
        return entityIdGen;
    }

    template<template<class> class ETNode, class ETType>
    void connectNode(ETNode<ETType>* node, EntityId addressId) {
        auto etId = GetTypeId<ETType>();
        auto& connections = activeConnection[etId];
        for(const auto& currConn: connections) {
            if(currConn.node == node) {
                return;
            }
        }
        ETConnection conn;
        conn.node = node;
        conn.addressId = addressId;
        connections.push_back(conn);
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>* node) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        auto& etConnection = it->second;
        auto removeIt = etConnection.end();
        for(auto etIt = etConnection.begin(), etEnd = etConnection.end(); etIt != etEnd; ++etIt) {
            if(etIt->node == node) {
                etConnection.erase(etIt);
                break;
            }
        }
        if(etConnection.empty()) {
            activeConnection.erase(it);
        }
    }

    // ==--------------- Const ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                return;
            }
        }
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            auto obj = static_cast<ETNode<ETType>*>(currConn.node);
            (obj->*func)(std::forward<ParamType>(params)...);
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            auto obj = static_cast<ETNode<ETType>*>(currConn.node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
            return;
        }
    }

    // ==--------------- Regular ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
            }
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                retVal = (obj->*func)(std::forward<ParamType>(params)...);
                return;
            }
        }
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            auto obj = static_cast<ETNode<ETType>*>(currConn.node);
            (obj->*func)(std::forward<ParamType>(params)...);
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            auto obj = static_cast<ETNode<ETType>*>(currConn.node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
            return;
        }
    }

private:

    ETSystem(const ETSystem&) = delete;
    ETSystem& operator=(const ETSystem&) = delete;

private:

    typedef std::vector<ETConnection> ConnectionArrayT;
    std::unordered_map<EntityId, ConnectionArrayT> activeConnection;

    EntityId entityIdGen;
};

#endif /* __ET_SYSTEM_HPP__ */