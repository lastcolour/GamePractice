#ifndef __ET_PRIMITIVES_HPP__
#define __ET_PRIMITIVES_HPP__

#include "Core/ETSystem.hpp"

#include <vector>
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
        if(addressId == adId) {
            return;
        } else if(addressId != adId && addressId != InvalidEntityId) {
            disconnect();
        }
        if(adId != InvalidEntityId) {
            GetETSystem()->connectNode(*this, adId);
        }
        addressId = adId;
    }
    void disconnect() {
        if(addressId == InvalidEntityId) {
            return;
        }
        GetETSystem()->disconnectNode(*this);
        addressId.setInvalidId();
    }

private:

    ETNode(const ETNode&) = delete;
    ETNode& operator=(const ETNode&) = delete;

private:

    EntityId addressId;
};

// ==--------------- Const ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
}

// ==--------------- Regural ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEvent(method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    GetETSystem()->sendEvent(addressId, method, std::forward<ParamType>(params)...);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(!std::is_same<RetType, void>::value, "Can't return void type");
    GetETSystem()->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
}

template<typename ETType>
bool ET_IsExistNode() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetETSystem()->isExistNode<ETType>(SystemEntityId);
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

#endif /* __ET_PRIMITIVES_HPP__ */