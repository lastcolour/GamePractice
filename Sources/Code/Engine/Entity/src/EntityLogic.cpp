#include "Entity/EntityLogic.hpp"
#include "Entity.hpp"

#include <cassert>

EntityLogic::EntityLogic() :
    entity(nullptr) {
}

EntityLogic::~EntityLogic() {
}

EntityId EntityLogic::getEntityId() const {
    if(entity) {
        return entity->getEntityId();
    }
    assert(false && "No valid entity to query Id");
    return InvalidEntityId;
}

const Transform& EntityLogic::getTransform() const {
    return entity->ET_getTransform();
}

void EntityLogic::setTransform(const Transform& newTm) {
    entity->ET_setTransform(newTm);
}

const char* EntityLogic::getEntityName() const {
    return entity->ET_getName();
}

Transform EntityLogic::getLocalTransform() const {
    return entity->ET_getLocalTransform();
}

void EntityLogic::setLocalTransform(const Transform& newLocTm) {
    entity->ET_setLocalTransform(newLocTm);
}