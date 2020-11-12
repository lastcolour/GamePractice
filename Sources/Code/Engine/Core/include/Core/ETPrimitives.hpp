#ifndef __ET_PRIMITIVES_HPP__
#define __ET_PRIMITIVES_HPP__

#include "Core/ETSystem.hpp"

#include <type_traits>

class ETNodeBase {
protected:
    virtual ~ETNodeBase() = default;
};

template<typename T>
class ETNode : public ETNodeBase, public T {

    friend class ETSystem;

public:

    ETNode() {
        static_assert(std::is_abstract<T>::value, "ETType can be only abstract class");
    }
    virtual ~ETNode() {
        disconnect();
    }

protected:

    void connect(EntityId adId) {
        GetETSystem()->connectNode(*this, adId);
    }
    void disconnect() {
        GetETSystem()->disconnectNode(*this);
    }

private:

    ETNode(const ETNode&) = delete;
    ETNode& operator=(const ETNode&) = delete;
};

// ==--------------- Const ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
}

// ==--------------- Regural ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(!std::is_same<RetType, void>::value, "Can't return void type");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");
    GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
}

template<typename ETType>
bool ET_IsExistNode() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return !GetETSystem()->getAll<ETType>().empty();
}

template<typename ETType>
bool ET_IsExistNode(EntityId addressId) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetETSystem()->isExistNode<ETType>(addressId);
}

template<typename ETType>
std::vector<EntityId> ET_GetAll() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetETSystem()->getAll<ETType>();
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->queueEvent(method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->queueEvent(method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->queueEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    GetETSystem()->queueEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ETType>
void ET_PollAllEvents() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetETSystem()->pollAllEvents<ETType>();
}

#endif /* __ET_PRIMITIVES_HPP__ */