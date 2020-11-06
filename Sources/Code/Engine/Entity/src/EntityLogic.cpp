#include "Entity/EntityLogic.hpp"
#include "Entity.hpp"

#include <cassert>

EntityLogic::EntityLogic() :
    entity(nullptr) {
}

EntityLogic::~EntityLogic() {
}

void EntityLogic::setEntity(const Entity* ownEntity) {
    entity = ownEntity;
}

EntityId EntityLogic::getEntityId() const {
    if(entity) {
        return entity->getEntityId();
    }
    assert(false && "No valid entity to query Id");
    return InvalidEntityId;
}