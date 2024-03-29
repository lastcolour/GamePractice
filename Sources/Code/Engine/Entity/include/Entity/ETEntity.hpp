#ifndef __ET_ENTITY_HPP__
#define __ET_ENTITY_HPP__

#include "Math/Transform.hpp"

class EntityLogic;

using EntityChildId = int;
const EntityChildId InvalidEntityChildId = -1;

using EntityLogicId = int;
const EntityLogicId InvalidEntityLogicId = -1;

struct ETEntity {
    virtual ~ETEntity() = default;
    virtual void ET_setParent(EntityId entId) = 0;
    virtual EntityChildId ET_addChild(EntityId entId) = 0;
    virtual void ET_removeChild(EntityId entId) = 0;
    virtual EntityId ET_getParentId() const = 0;
    virtual const char* ET_getName() const = 0;
    virtual const Transform& ET_getTransform() const = 0;
    virtual void ET_setTransform(const Transform& tm) = 0;
    virtual Transform ET_getLocalTransform() const = 0;
    virtual void ET_setLocalTransform(const Transform& localTm) = 0;
    virtual EntityChildId ET_getChildIdFromEntityId(EntityId childEntId) const = 0;
    virtual EntityId ET_getEntityIdFromChildId(const std::vector<EntityChildId>& childrenIds) const = 0;
    virtual std::vector<EntityId> ET_getChildren() const = 0;
    virtual std::vector<EntityLogic*> ET_getLogisByTypeId(Core::TypeId logicTypeId) = 0;
};

namespace EntityUtils {

const char* GetEntityName(EntityId entityId);

std::vector<EntityLogic*> GetEntityLogicsByTypeId(Core::TypeId logicTypeId, EntityId entityId);

} // namespace EntityUtils

template<typename T>
std::vector<T*> GetEntityLogics(EntityId entityId) {
    static_assert(std::is_base_of<EntityLogic, T>::value,
        "Can't query non logic type");
    auto logics = EntityUtils::GetEntityLogicsByTypeId(Core::GetTypeId<T>(), entityId);
    std::vector<T*> result = std::move(reinterpret_cast<std::vector<T*>&>(logics));
    return result;
}

#endif /* __ET_ENTITY_HPP__ */