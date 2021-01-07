#ifndef __ENTITY_HPP__
#define __ENTITY_HPP__

#include "Entity/ETEntity.hpp"
#include "Core/ETPrimitives.hpp"
#include "Reflect/ClassInstance.hpp"

#include <string>
#include <memory>

class EntityLogic;
class EntityRegistry;

class Entity : public ETNode<ETEntity> {
public:

    struct EntityChildNode {
        Entity* childEntity;
        EntityChildId childId;
    };

    struct EntityLogicNode {
        ClassInstance logic;
        EntityLogicId logicId;
    };

public:

    Entity(const char* entityName, EntityRegistry* entityRegistry, EntityId entId);
    virtual ~Entity();

    void addChildEntityWithId(EntityChildId childId, Entity& entity);
    bool addLogicWithId(EntityLogicId logicId, ClassInstance&& logicInstance);
    EntityLogicId addLogic(ClassInstance&& logicInstance);
    bool removeLogic(EntityLogicId logicId);
    bool readLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool writeLogicData(EntityLogicId logicId, EntityLogicValueId valueId, MemoryStream& stream);
    bool addLogicValueArrayElemet(EntityLogicId logicId, EntityLogicValueId valueId);
    EntityId getEntityId() const { return entityId; }
    void setName(const char* newName);
    void* addLogicByTypeId(TypeId logicTypeId);
    void purgeAllRelationships();

    template<typename LogicType>
    LogicType* addCustomLogic() {
        static_assert(std::is_base_of<EntityLogic, LogicType>::value, "Invalid non-entity logic type");
        static_assert(!std::is_abstract<LogicType>::value, "Can't add logic of abstract type");
        LogicType* logicPtr = reinterpret_cast<LogicType*>(
            addLogicByTypeId(GetTypeId<LogicType>()));
        if(logicPtr == nullptr) {
            logicPtr = new LogicType();
            ClassInstance instance = ClassInstance::CreateWithoutClassInfo<LogicType>(logicPtr);
            addLogic(std::move(instance));
        }
        return logicPtr;
    }

    // ETEntity
    const char* ET_getName() const override;
    void ET_setParent(EntityId entId) override;
    EntityChildId ET_addChild(EntityId entId) override;
    void ET_removeChild(EntityId entId) override;
    EntityId ET_getParentId() const override;
    const Transform& ET_getTransform() const override;
    void ET_setTransform(const Transform& newTm) override;
    EntityChildId ET_getChildIdFromEntityId(EntityId childEntId) const override;
    EntityId ET_getEntityIdFromChildId(EntityChildId childId) const override;
    Transform ET_getLocalTransform() const override;
    void ET_setLocalTransform(const Transform& newLocalTm) override;
    std::vector<EntityId> ET_getChildren() const override;
    std::vector<EntityLogic*> ET_getLogisByTypeId(TypeId logicTypeId) override;

private:

    Entity& operator=(const Entity&) = delete;
    Entity(const Entity&) = delete;

private:

    ClassInstance* findLogic(EntityLogicId logicId);
    EntityLogicId createNewLogicId() const;
    EntityChildId createNewChildId() const;

private:

    EntityRegistry* registry;
    Entity* parent;
    Transform tm;
    std::vector<EntityLogicNode> logics;
    std::vector<EntityChildNode> children;
    std::string name;
    EntityId entityId;
};

#endif /* __ENTITY_HPP__ */