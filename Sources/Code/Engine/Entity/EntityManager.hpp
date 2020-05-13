#ifndef __ENTITY_MANAGER_HPP__
#define __ENTITY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Entity;
class EntityLogic;
class JSONNode;
class ClassInfo;

class EntityManager : public SystemLogic,
    public ETNode<ETEntityManager> {

    using EntityPtrT = std::unique_ptr<Entity>;
    using LogicPtrT = std::unique_ptr<EntityLogic>;

public:

    EntityManager();
    virtual ~EntityManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETEntityManager
    EntityId ET_createEntity(const char* entityName) override;
    void ET_destroyEntity(EntityId entityId) override;
    bool ET_registerLogics(EntityLogicRegister& logicRegister) override;

private:

    Entity* createEntityImpl(Entity* rootEntity, const char* entityName);
    Entity* createEntity(Entity* rootEntity, const char* entityName);
    JSONNode loadEntityRootNode(const char* entityName) const;
    bool setupEntityLogics(Entity* entity, const JSONNode& node, const char* entityName) const;
    bool setupEntityChildren(Entity* entity, const JSONNode& node, const char* entityName);

private:

    std::unordered_map<std::string, ClassInfo*> registeredLogics;
    std::vector<EntityPtrT> entities;
};

#endif /* __ENTITY_MANAGER_HPP__ */