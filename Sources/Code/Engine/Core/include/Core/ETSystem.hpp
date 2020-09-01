#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/ETNodeRegistry.hpp"
#include "Core/ETThreadSafe.hpp"

#include <mutex>

namespace ET {

int GetNextETId();

int GetTotalETIds();

template<typename T>
class ETIdHelper {
public:
    static int value() {
        static const int etId = GetNextETId();
        return etId;
    }
};

template<typename T>
constexpr int GetETId() {
    return ETIdHelper<T>::value();
}

} // namespace ET

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
        return registry.getAll(ET::GetETId<ETType>());
    }

    template<typename ETType>
    bool isExistNode(EntityId addressId) {
        return registry.isExist(ET::GetETId<ETType>(), addressId);
    }

    template<template<class> class ETNode, class ETType>
    void connectNode(ETNode<ETType>& node, EntityId addressId) {
        registry.connectNode(ET::GetETId<ETType>(), ET::IsThreadSafe<ETType>(), addressId, &node);
    }

    template<template<class> class ETNode, class ETType>
    void disconnectNode(ETNode<ETType>& node) {
        registry.disconnectNode(ET::GetETId<ETType>(), &node);
    }

    // ==--------------- Const ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forEachNode(ET::GetETId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forFirst(ET::GetETId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forEachNode(ET::GetETId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.forFirst(ET::GetETId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    // ==--------------- Regular ETs ---------------==

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forEachNode(ET::GetETId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forFirst(ET::GetETId<ETType>(), addressId, [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forEachNode(ET::GetETId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void sendEventReturn(ValRetType& retVal, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.forFirst(ET::GetETId<ETType>(), [&](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            retVal = (obj->*func)(std::forward<ParamType>(params)...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.queueEventForAddress(ET::GetETId<ETType>(), addressId, [=](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(params...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.queueEventForAddress(ET::GetETId<ETType>(), addressId, [=](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(params...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        registry.queueEventForAll(ET::GetETId<ETType>(), [=](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(params...);
        });
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        registry.queueEventForAll(ET::GetETId<ETType>(), [=](ETNodeBase* node){
            auto obj = static_cast<ETNode<ETType>*>(node);
            (obj->*func)(params...);
        });
    }

    template<typename ETType>
    void pollAllEvents() {
        registry.pollEventsForAll(ET::GetETId<ETType>());
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