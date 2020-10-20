#ifndef __ET_ENTITY_HPP__
#define __ET_ENTITY_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

struct ETEntityEvents {
    virtual ~ETEntityEvents() = default;
    virtual void ET_onTransformChanged(const Transform& newTm) = 0;
    virtual void ET_onLoaded() = 0;
};

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
    virtual EntityId ET_getEntityIdFromChildId(EntityChildId childId) const = 0;
    virtual std::vector<EntityId> ET_getChildren() const = 0;
};

namespace EntityUtils {

const char* GetEntityName(EntityId entityId);

} // namespace EntityUtils

#endif /* __ET_ENTITY_HPP__ */