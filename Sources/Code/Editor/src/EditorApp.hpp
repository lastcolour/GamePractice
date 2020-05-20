#ifndef __EDITOR_APP__
#define __EDITOR_APP__

#include "Application.hpp"
#include "Core/Buffer.hpp"
#include "Core/Core.hpp"
#include "Render/RenderTextureFramebuffer.hpp"

class EditorApp : public Application {
public:

    EditorApp();
    virtual ~EditorApp();

    bool initialize();
    void deinitiazlie();
    const char* getReflectModel();
    std::vector<const char*> getRegisteredEntityLogics();
    EntityId loadEntity(const char* entityName);
    void unloadEntity(EntityId entityId);
    std::vector<EntityId> getEntityChildren(EntityId entityId);
    const char* getEntityName(EntityId entityId);
    EntityLogicId addLogicToEntity(EntityId entityId, const char* logicName);
    void removeLogicFromEntity(EntityId entityId, EntityLogicId logicId);
    Buffer getEntityLogicData(EntityId entityId, EntityLogicId logicId);
    void drawFrame(void* out, int32_t width, int32_t height);
    EntityId addChilEntityToEntity(EntityId entityId, const char* childName);
    void removeChildEntityFromEntity(EntityId parentId, EntityId childId);
    void setEntityLogicFieldData(EntityId entityId, EntityLogicId logicId, int fieldId, Buffer& buffer);

protected:

    void buildModules(ModuleListT& modules) override;

private:

    Buffer reflectModelBuffer;
    EntityId centralEntityId;
    RenderTextureFramebuffer frameBuffer;
};

#endif /* __EDITOR_APP__ */