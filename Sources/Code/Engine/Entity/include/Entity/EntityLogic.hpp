#ifndef __ENTITY_LOGIC_HPP__
#define __ENTITY_LOGIC_HPP__

#include "Math/Transform.hpp"

class Entity;

class EntityLogic {
public:

    virtual ~EntityLogic();

    virtual void init() = 0;
    virtual void deinit() = 0;

    virtual void onLoaded() {}
    virtual void onTransformChanged(const Transform& newTm) {}

    EntityId getEntityId() const;
    const char* getEntityName() const;

    const Transform& getTransform() const;
    void setTransform(const Transform& newTm);

    Transform getLocalTransform() const;
    void setLocalTransform(const Transform& newLocTm);

protected:

    EntityLogic();

private:

    friend class Entity;

    Entity* entity;
};

#endif /* __ENTITY_LOGIC_HPP__ */