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

        auto etId = GetTypeId<ETType>();
        auto& connections = activeConnection[etId];
        for(const auto& currConn: connections) {
            if(currConn.node == &node) {
                return;
            }
        }
        ETConnection conn;
        conn.node = &node;
        conn.addressId = addressId;
        connections.push_back(conn);
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>& node) {
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        auto& etConnection = it->second;
        for(auto etIt = etConnection.begin(), etEnd = etConnection.end(); etIt != etEnd; ++etIt) {
            if(etIt->node == &node) {
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
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
                return;
            }
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        LockTypeT lock(mutex);

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
        LockTypeT lock(mutex);

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
        LockTypeT lock(mutex);

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
        LockTypeT lock(mutex);

        auto etId = GetTypeId<ETType>();
        auto it = activeConnection.find(etId);
        if(it == activeConnection.end()) {
            return;
        }
        for(auto& currConn : it->second) {
            if(currConn.addressId == addressId) {
                auto obj = static_cast<ETNode<ETType>*>(currConn.node);
                (obj->*func)(std::forward<ParamType>(params)...);
                return;
            }
        }
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        LockTypeT lock(mutex);

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
        LockTypeT lock(mutex);

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
        LockTypeT lock(mutex);
    
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

    TypeConnectionMapT activeConnection;
    mutable MutexTypeT mutex;
    EntityId entityIdGen;
};

ETSystem* GetETSystem();

#endif /* __ET_SYSTEM_HPP__ */