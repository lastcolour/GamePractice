#ifndef __ET_PRIMITIVES_HPP__
#define __ET_PRIMITIVES_HPP__

#include "Environment.hpp"
#include "Core/ETSystem.hpp"

#include <vector>
#include <type_traits>

class ETNodeBase {
protected:
    virtual ~ETNodeBase() = default;
};

template<typename T>
class ETNode : public ETNodeBase, public T {
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
        addressId = adId;
        if(auto et = GetEnv()->getETSystem()) {
            et->connectNode(this, addressId);
        }
    }
    void disconnect() {
        if(addressId == InvalidEntityId) {
            return;
        }
        if(auto env = GetEnv()) {
            if(auto et = env->getETSystem()) {
                et->disconnectNode(this);
            }
        }
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
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEvent(method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
    }
}

// ==--------------- Regural ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEvent(method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEventReturn(retVal, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEvent(addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    if(auto et = GetEnv()->getETSystem()) {
        et->sendEventReturn(retVal, addressId, method, std::forward<ParamType>(params)...);
    }
}

template<typename ETType>
bool ET_IsExistNode(EntityId addressId) {
    if(auto et = GetEnv()->getETSystem()) {
        return et->isExistNode<ETType>(addressId);
    }
    return false;
}
template<typename ETType>
std::vector<EntityId> ET_GetAll() {
    if(auto et = GetEnv()->getETSystem()) {
        return et->getAll<ETType>();
    }
    return std::vector<EntityId>();
}

#endif /* __ET_PRIMITIVES_HPP__ */