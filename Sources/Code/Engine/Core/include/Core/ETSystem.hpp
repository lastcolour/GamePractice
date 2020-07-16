#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/ETNodeRegistry.hpp"

#include <type_traits>
#include <mutex>

template<typename T>
class ETNode;

class ETSystem {
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
    std::vector<EntityId> getAll() {
        return registry.getAll(GetTypeId<ETType>());
    }

    template<typename ETType>
    bool isExistNode(EntityId addressId) {
        return registry.isExist(GetTypeId<ETType>(), addressId);
    }

    template<template<class> class ETNode, class ETType>
    void connectNode(ETNode<ETType>& node, EntityId addressId) {
        registry.connectNode(GetTypeId<ETType>(), addressId, &node);
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>& node) {
        registry.disconnectNode(GetTypeId<ETType>(), &node);
    }

    // ==--------------- Const ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forEachNode(GetTypeId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forFirst(GetTypeId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forEachNode(GetTypeId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forFirst(GetTypeId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    // ==--------------- Regular ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forEachNode(GetTypeId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forFirst(GetTypeId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forEachNode(GetTypeId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forFirst(GetTypeId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

private:

    ETSystem(const ETSystem&) = delete;
    ETSystem& operator=(const ETSystem&) = delete;

private:

    ETNodeRegistry registry;
    std::mutex idGenMutex;
    EntityId entityIdGen;
};

ETSystem* GetETSystem();

#endif /* __ET_SYSTEM_HPP__ */