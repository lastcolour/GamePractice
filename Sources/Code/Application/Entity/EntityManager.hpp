#ifndef __ENTITY_MANAGER_HPP__
#define __ENTITY_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Entity/ETEntityInterfaces.hpp"

#include <memory>
#include <string>
#include <unordered_map>

class Entity;
class EntityLogic;

class EntityManager : public SystemLogic,
    public ETNode<ETEntityManager> {

    using EntityPtrT = std::unique_ptr<Entity>;
    using LogicPtrT = std::unique_ptr<EntityLogic>;
    typedef std::unique_ptr<EntityLogic>(*LogicCreateFunc)();

public:

    EntityManager();
    virtual ~EntityManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETEntityManager
    EntityId ET_createEntity(const char* entityName) override;
    bool ET_extendEntity(EntityId entId, const char* entityName) override;
    void ET_destroyEntity(EntityId entId) override;

private:

    EntityPtrT createEntity(Entity* rootEnt, const char* entityName);

    template<typename T>
    void registerLogic(const char* logicName) {
        auto createFunc = [](){
            return LogicPtrT(new T);
        };
        registerCreateLogic(logicName, createFunc);
    }

    void registerCreateLogic(const char* logicName, LogicCreateFunc createFunc);
    LogicPtrT createLogic(const char* logicName);

private:

    std::unordered_map<std::string, LogicCreateFunc> logics;
    std::vector<EntityPtrT> entities;
};

#endif /* __ENTITY_MANAGER_HPP__ */