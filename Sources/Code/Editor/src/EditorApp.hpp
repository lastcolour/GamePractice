#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Platform/ETSurface.hpp"

class TasksRunner;

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    Buffer getReflectModel();
    Buffer getRegisteredEntityLogics();
    EntityId loadEntityFromFile(const char* entityName);
    EntityId loadEntityFromData(const char* entityName, const char* entityData);
    void unloadEntity(EntityId entityId);
    EntityId getEntityChildEntityId(EntityId entityId, EntityChildId childId);
    const char* getEntityName(EntityId entityId);
    EntityLogicId addLogicToEntity(EntityId entityId, const char* logicName);
    void removeLogicFromEntity(EntityId entityId, EntityLogicId logicId);
    void drawFrame(void* out, int32_t width, int32_t height);
    EntityChildId addChilEntityToEntity(EntityId parentId, EntityId childId);
    void removeChildEntityFromEntity(EntityId parentId, EntityId childId);
    void addEntityLogicArrayElement(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId);
    void setEntityLogicPolymorphObjectType(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId, const char* newType);
    Buffer getEntityLogicData(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId);
    void setEntityLogicData(EntityId entityId, EntityLogicId logicId, EntityLogicValueId valueId, Buffer& buffer);
    EntityChildId createChildEntity(EntityId entityId, const char* childName);
    void mouseInputEvent(EActionType actionType, const Vec2i& pos);
    void setTimeScale(float timeScale);
    void enableGameUpdate(bool flag);
    void unloadAll();
    bool renameEntity(EntityId entityId, const char* newName);
    void setFocusEntity(EntityId entityId);

protected:

    void buildModules(ModuleListT& modules) override;
    void buildTasksRunner();

private:

    bool updateGame;
};

#endif /* __EDITOR_APP__ */