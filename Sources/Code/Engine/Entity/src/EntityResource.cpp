#include "Entity/EntityResource.hpp"
#include "Entity/ETEntityManager.hpp"

EntityId EntityResource::createInstance() const {
    EntityId entId;
    if(isValid()) {
        ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, get().c_str());
    }
    return entId;
}

const char* EntityResource::getPath() const {
    if(!isSet()) {
        return "null";
    } else {
        return get().c_str();
    }
}

void EntityResource::setPath(const char* newPath) {
    if(!newPath || !newPath[0]) {
        reset();
    } else {
        set(newPath);
    }
}