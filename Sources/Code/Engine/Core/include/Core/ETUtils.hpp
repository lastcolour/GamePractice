#ifndef __ET_UTILS_HPP__
#define __ET_UTILS_HPP__

class EntityId {

    static const uint32_t INVALID_ID = 0u;

public:

    EntityId() : id(INVALID_ID) {}
    EntityId(const EntityId& entId) : id(entId.id) {}
    EntityId& operator=(const EntityId& endId) { id = endId.id; return *this; }
    ~EntityId() = default;
    bool operator==(const EntityId& entId) const { return id == entId.id; }
    bool operator!=(const EntityId& entId) const { return id != entId.id; }
    bool isValid() const { return id != INVALID_ID; }

    void setRawId(uint32_t entId) { id = entId; }
    uint32_t getRawId() const { return id; }

private:

    uint32_t id;
};

namespace std {
    template <>
    struct hash<EntityId> {
        size_t operator()(const EntityId& entId) const {
            return std::hash<uint32_t>()(entId.getRawId());
        }
    };
}

const EntityId InvalidEntityId;

namespace ET {

class ETNodeBase;

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
