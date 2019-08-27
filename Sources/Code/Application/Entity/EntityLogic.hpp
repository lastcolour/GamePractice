#ifndef __ENTITY_LOGIC_HPP__
#define __ENTITY_LOGIC_HPP__

#include "Core/Core.hpp"

class Entity;
class JSONNode;

class EntityLogic {
public:

    virtual ~EntityLogic();

    virtual bool serialize(const JSONNode& node) = 0;
    virtual bool init() = 0;

    void setEntity(const Entity* ownEntity);
    EntityId getEntityId() const;
    const char* getEntityName() const;
    EntityId getParentId() const;

protected:

    EntityLogic();

private:

    const Entity* entity;
};

#endif /* __ENTITY_LOGIC_HPP__ */