#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/ETNodeRegistry.hpp"
#include "Core/GlobalEnvironment.hpp"

namespace ET {

class ETSystem {
public:

    ETSystem() = default;
    ~ETSystem() = default;

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
        registry.connectNode(ET::GetETId<ETType>(), addressId, &node);
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
        using ObjectT = ETNode<ETType>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(func), ParamType...>;

        registry.queueEvent(ET::GetETId<ETType>(),
            registry.createDefferedEvent<CallT>(addressId, func, std::forward<ParamType>(params)...));
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(EntityId addressId, RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        using ObjectT = ETNode<ETType>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(func), ParamType...>;

        registry.queueEvent(ET::GetETId<ETType>(),
            registry.createDefferedEvent<CallT>(addressId, func, std::forward<ParamType>(params)...));
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(RetType (ETType::*func)(ArgsType...) const, ParamType&& ... params) {
        using ObjectT = ETNode<ETType>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(func), ParamType...>;

        registry.queueEvent(ET::GetETId<ETType>(),
            registry.createDefferedEvent<CallT>(InvalidEntityId, func, std::forward<ParamType>(params)...));
    }

    template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
    void queueEvent(RetType (ETType::*func)(ArgsType...), ParamType&& ... params) {
        using ObjectT = ETNode<ETType>;
        using CallT = ET::ETDefferedCall<ObjectT, decltype(func), ParamType...>;

        registry.queueEvent(ET::GetETId<ETType>(),
            registry.createDefferedEvent<CallT>(InvalidEntityId, func, std::forward<ParamType>(params)...));
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

} // namespace ET

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetEnv()->GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetEnv()->GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
    }
}

// ==--------------- Regural ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetEnv()->GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetEnv()->GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(!std::is_same<RetType, void>::value, "Can't return void type");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType>
bool ET_IsExistNode() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return !GetEnv()->GetETSystem()->getAll<ETType>().empty();
}

template<typename ETType>
bool ET_IsExistNode(EntityId addressId) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    if(addressId.isValid()) {
        return GetEnv()->GetETSystem()->isExistNode<ETType>(addressId);
    }
    return false;
}

template<typename ETType>
std::vector<EntityId> ET_GetAll() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetEnv()->GetETSystem()->getAll<ETType>();
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetEnv()->GetETSystem()->queueEvent(method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetEnv()->GetETSystem()->queueEvent(method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->queueEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    if(addressId.isValid()) {
        GetEnv()->GetETSystem()->queueEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType>
void ET_PollAllEvents() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetEnv()->GetETSystem()->pollAllEvents<ETType>();
}

#endif /* __ET_SYSTEM_HPP__ */