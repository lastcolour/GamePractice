#ifndef __CORE_HPP__
#define __CORE_HPP__

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

using EntityLogicId = int;
const EntityLogicId InvalidEntityLogicId = -1;

using EntityChildId = int;
const EntityChildId InvalidEntityChildId = -1;

using EntityLogicValueId = int;
const EntityLogicValueId InvalidEntityLogicValueId = -1;
const EntityLogicValueId AllEntityLogicValueId = 0;

#endif /* __CORE_HPP__ */