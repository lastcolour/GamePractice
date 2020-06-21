#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Core/Buffer.hpp"
#include "Core/Core.hpp"
#include "Platform/ETSurface.hpp"
#include "Render/RenderTextureFramebuffer.hpp"

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    Buffer getReflectModel();
    Buffer getRegisteredEntityLogics();
    EntityId loadEntity(const char* entityName);
    void unloadEntity(EntityId entityId);
    EntityId getEntityChildEntityId(EntityId entityId, EntityChildId childId);
    const char* getEntityName(EntityId entityId);
    EntityLogicId addLogicToEntity(EntityId entityId, const char* logicName);
    void removeLogicFromEntity(EntityId entityId, EntityLogicId logicId);
    void drawFrame(void* out, int32_t width, int32_t height);
    EntityChildId addChilEntityToEntity(EntityId parentId, EntityId childId);
    void removeChildEntityFromEntity(EntityId parentId, EntityId childId);
    void addEntityLogicArrayElement(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId);
    Buffer getEntityLogicData(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId);
    void setEntityLogicData(EntityId entityId, EntityLogicId logicId, EntityLogicValueId valueId, Buffer& buffer);
    EntityChildId createChildEntity(EntityId entityId, const char* childName);
    void mouseInputEvent(EActionType actionType, const Vec2i& pos);
    void setTimeScale(float timeScale);
    void enableGameUpdate(bool flag);
    void unloadAll();
    bool renameEntity(EntityId entityId, const char* newName);

protected:

    void buildModules(ModuleListT& modules) override;

private:

    RenderTextureFramebuffer frameBuffer;
    bool updateGame;
};

#endif /* __EDITOR_APP__ */