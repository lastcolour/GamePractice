#ifndef __CORE_HPP__
#define __CORE_HPP__

#include <functional>

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
    void setInvalidId() { id = INVALID_ID; }

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

typedef int TypeId;
const TypeId InvalidTypeId = 0;

#endif /* __CORE_HPP__ */