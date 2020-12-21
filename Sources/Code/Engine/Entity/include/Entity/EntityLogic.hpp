#ifndef __ENTITY_LOGIC_HPP__
#define __ENTITY_LOGIC_HPP__

#include "Core/Core.hpp"

class Entity;

class EntityLogic {
public:

    virtual ~EntityLogic();

    virtual void init() = 0;
    virtual void deinit() = 0;

    void setEntity(const Entity* ownEntity);
    EntityId getEntityId() const;

protected:

    EntityLogic();

private:

    const Entity* entity;
};

#endif /* __ENTITY_LOGIC_HPP__ */