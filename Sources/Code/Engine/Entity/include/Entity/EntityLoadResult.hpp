#ifndef __ENTITY_LOAD_RESULT_HPP__
#define __ENTITY_LOAD_RESULT_HPP__

#include "Entity/ETEntityManger.hpp"

class EntityLoadResult {
public:

    EntityLoadResult(EntityId loadedEntId);
    EntityLoadResult(const EntityLoadResult&& other);
    EntityLoadResult& operator=(EntityLoadResult&& other);
    ~EntityLoadResult();

    EntityId finishLoad();

private:

    EntityId entityId;
    int refCount;
};

#endif /* __ENTITY_LOAD_RESULT_HPP__ */