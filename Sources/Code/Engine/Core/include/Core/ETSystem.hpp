#ifndef __ET_SYSTEM_HPP__
#define __ET_SYSTEM_HPP__

#include "Core/ETNodeRegistry.hpp"

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

    void connectNode(int etId, ETNodeBase* node, EntityId addressId) {
        registry.connectNode(etId, addressId, node);
    }

    void disconnectNode(int etId, ETNodeBase* node) {
        registry.disconnectNode(etId, node);
    }

    template<typename ETType>
    std::vector<EntityId> getAll() {
        return registry.getAll(ET::GetETId<ETType>());
    }

    template<typename ETType>
    bool isExistNode(EntityId addressId) {
        return registry.isExist(ET::GetETId<ETType>(), addressId);
    }

    void queueEvent(int etId, ETDefferedCallBase* event) {
        registry.queueEvent(etId, event);
    }

    template<typename ETType>
    void pollAllEvents() {
        registry.pollEventsForAll(ET::GetETId<ETType>());
    }

    ETIterator createIterator(int etId) {
        return registry.createIterator(etId);
    }

    ETIterator createIterator(int etId, EntityId adressId) {
        return registry.createIterator(etId, adressId);
    }

    void destroyIterator(const ETIterator& it) {
        registry.destroyIterator(it);
    }

    template<typename T, typename ... ArgsType>
    T* createDefferedEvent(ArgsType&& ... args) {
        auto ptr = GetEnv()->GetMemoryAllocator()->allocate(sizeof(T));
        return new (ptr) T(std::forward<ArgsType>(args)...);
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

template<typename T>
class ETNode : public ET::ETNodeBase, public T {

    friend class ET::ETSystem;

public:

    ETNode() {
        static_assert(std::is_abstract<T>::value, "ETType can be only abstract class");
    }
    virtual ~ETNode() {
        disconnect();
    }

protected:

    void connect(EntityId adId) {
        GetEnv()->GetETSystem()->connectNode(ET::GetETId<T>(), this, adId);
    }
    void disconnect() {
        GetEnv()->GetETSystem()->disconnectNode(ET::GetETId<T>(), this);
    }

private:

    ETNode(const ETNode&) = delete;
    ETNode& operator=(const ETNode&) = delete;
};

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    auto etSystem = GetEnv()->GetETSystem();
    auto it = etSystem->createIterator(ET::GetETId<ETType>());
    if(it) {
        auto obj = static_cast<ETNode<ETType>*>(*it);
        (obj->*method)(std::forward<ParamType>(params)...);
    }
    etSystem->destroyIterator(it);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");

    auto etSystem = GetEnv()->GetETSystem();
    auto it = etSystem->createIterator(ET::GetETId<ETType>());
    if(it) {
        auto obj = static_cast<ETNode<ETType>*>(*it);
        retVal = (obj->*method)(std::forward<ParamType>(params)...);
    }
    etSystem->destroyIterator(it);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    if(addressId.isValid()) {
        auto etSystem = GetEnv()->GetETSystem();
        auto it = etSystem->createIterator(ET::GetETId<ETType>(), addressId);
        while(it) {
            auto obj = static_cast<ETNode<ETType>*>(*it);
            (obj->*method)(std::forward<ParamType>(params)...);
            ++it;
        }
        etSystem->destroyIterator(it);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");

    if(addressId.isValid()) {
        auto etSystem = GetEnv()->GetETSystem();
        auto it = etSystem->createIterator(ET::GetETId<ETType>(), addressId);
        if(it) {
            auto obj = static_cast<ETNode<ETType>*>(*it);
            retVal = (obj->*method)(std::forward<ParamType>(params)...);
        }
        etSystem->destroyIterator(it);
    }
}

// ==--------------- Regural ETs ---------------==

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    auto etSystem = GetEnv()->GetETSystem();
    auto it = etSystem->createIterator(ET::GetETId<ETType>());
    while(it) {
        auto obj = static_cast<ETNode<ETType>*>(*it);
        (obj->*method)(std::forward<ParamType>(params)...);
        ++it;
    }
    etSystem->destroyIterator(it);
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");

    auto etSystem = GetEnv()->GetETSystem();
    auto it = etSystem->createIterator(ET::GetETId<ETType>());
    if(it) {
        auto obj = static_cast<ETNode<ETType>*>(*it);
        retVal = (obj->*method)(std::forward<ParamType>(params)...);
    }
    etSystem->destroyIterator(it);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    if(addressId.isValid()) {
        auto etSystem = GetEnv()->GetETSystem();
        auto it = etSystem->createIterator(ET::GetETId<ETType>(), addressId);
        while(it) {
            auto obj = static_cast<ETNode<ETType>*>(*it);
            (obj->*method)(std::forward<ParamType>(params)...);
            ++it;
        }
        etSystem->destroyIterator(it);
    }
}

template<typename ValRetType, typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_SendEventReturn(ValRetType& retVal, EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(!std::is_same<RetType, void>::value, "Can't return void type");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");
    static_assert(!std::is_same<RetType, void>::value, "The ET function has void return type");

    if(addressId.isValid()) {
        auto etSystem = GetEnv()->GetETSystem();
        auto it = etSystem->createIterator(ET::GetETId<ETType>(), addressId);
        if(it) {
            auto obj = static_cast<ETNode<ETType>*>(*it);
            retVal = (obj->*method)(std::forward<ParamType>(params)...);
        }
        etSystem->destroyIterator(it);
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

    using ObjectT = ETNode<ETType>;
    using CallT = ET::ETDefferedCall<ObjectT, decltype(method), ParamType...>;

    auto etSystem = GetEnv()->GetETSystem();

    auto event = etSystem->createDefferedEvent<CallT>(InvalidEntityId, method, std::forward<ParamType>(params)...);
    etSystem->queueEvent(ET::GetETId<ETType>(), event);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    using ObjectT = ETNode<ETType>;
    using CallT = ET::ETDefferedCall<ObjectT, decltype(method), ParamType...>;

    auto etSystem = GetEnv()->GetETSystem();

    auto event = etSystem->createDefferedEvent<CallT>(InvalidEntityId, method, std::forward<ParamType>(params)...);
    etSystem->queueEvent(ET::GetETId<ETType>(), event);
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...), ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    using ObjectT = ETNode<ETType>;
    using CallT = ET::ETDefferedCall<ObjectT, decltype(method), ParamType...>;

    auto etSystem = GetEnv()->GetETSystem();

    if(addressId.isValid()) {
        auto event = etSystem->createDefferedEvent<CallT>(addressId, method, std::forward<ParamType>(params)...);
        etSystem->queueEvent(ET::GetETId<ETType>(), event);
    }
}

template<typename ETType, typename RetType, typename ... ArgsType, typename ... ParamType>
void ET_QueueEvent(EntityId addressId, RetType (ETType::*method)(ArgsType...) const, ParamType&& ... params) {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    static_assert(sizeof...(ArgsType) == sizeof...(ParamType), "Diffrent argument count");

    using ObjectT = ETNode<ETType>;
    using CallT = ET::ETDefferedCall<ObjectT, decltype(method), ParamType...>;

    auto etSystem = GetEnv()->GetETSystem();

    if(addressId.isValid()) {
        auto event = etSystem->createDefferedEvent<CallT>(addressId, method, std::forward<ParamType>(params)...);
        etSystem->queueEvent(ET::GetETId<ETType>(), event);
    }
}

template<typename ETType>
void ET_PollAllEvents() {
    static_assert(std::is_abstract<ETType>::value, "ETType can be only abstract class");
    return GetEnv()->GetETSystem()->pollAllEvents<ETType>();
}

#endif /* __ET_SYSTEM_HPP__ */