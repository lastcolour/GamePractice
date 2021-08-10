#ifndef __ET_UTILS_HPP__
#define __ET_UTILS_HPP__

#include "Core/Core.hpp"

class ETNodeBase;

template<typename T>
class ETNode;

namespace ET {

int GetNextETId();

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

struct ETDefferedCallBase {
public:

    ETDefferedCallBase(EntityId id) : addressId(id) {}
    virtual ~ETDefferedCallBase() = default;

    virtual void call(ETNodeBase* node) = 0;

public:

    EntityId addressId;
};

constexpr const size_t MAX_EVENT_SIZE = 164;

template<typename ObjectType, typename FuncType, typename ... ParamType>
class ETDefferedCall : public ETDefferedCallBase {

    using TupleType = std::tuple<ObjectType*, typename std::remove_reference<ParamType>::type...>;

public:

    ETDefferedCall(EntityId entId, FuncType callFunc, ParamType ... params) :
        ETDefferedCallBase(entId),
        args(nullptr, std::forward<ParamType>(params)...),
        func(callFunc) {
    }

    void call(ETNodeBase* node) override {
        std::get<0>(args) = static_cast<ObjectType*>(node);
        std::apply(func, args);
    }

private:

    TupleType args;
    FuncType func;
};

} // namespace ET

#endif /* __ET_UTILS_HPP__ */
