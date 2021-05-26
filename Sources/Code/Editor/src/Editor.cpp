#include "Editor.hpp"
#include "EditorApp.hpp"
#include "Core/MemoryStream.hpp"

EditorApp* EDITOR_APP = nullptr;
Buffer INTERNAL_BUFFER;

uint32_t Initiliaze() {
    if(EDITOR_APP) {
        return 1u;
    }
    EDITOR_APP = new EditorApp();
    if(!EDITOR_APP) {
        return 2u;
    }
    if(!EDITOR_APP->initialize()) {
        delete EDITOR_APP;
        EDITOR_APP = nullptr;
        return 3u;
    }
    return 0u;
}

void DeInitialize() {
    if (!EDITOR_APP) {
        return;
    }
    EDITOR_APP->deinitiazlie();
    delete EDITOR_APP;
    EDITOR_APP = nullptr;
}

const char* GetReflectModel() {
    if(!EDITOR_APP) {
        return nullptr;
    }
    INTERNAL_BUFFER = EDITOR_APP->getReflectModel();
    return INTERNAL_BUFFER.getCString();
}

const char* GetRegisteredEntityLogics() {
    if(!EDITOR_APP) {
        return 0u;
    }
    INTERNAL_BUFFER = EDITOR_APP->getRegisteredEntityLogics();
    return INTERNAL_BUFFER.getCString();
}

uint32_t LoadEntityFromFile(const char* entityName) {
    if(!EDITOR_APP) {
        return InvalidEntityId.getRawId();
    }
    auto entityId = EDITOR_APP->loadEntityFromFile(entityName);
    return entityId.getRawId();
}

uint32_t LoadEntityFromData(const char* entityName, const char* entityData) {
    if(!EDITOR_APP) {
        return InvalidEntityId.getRawId();
    }
    auto entityId = EDITOR_APP->loadEntityFromData(entityName, entityData);
    return entityId.getRawId();
}

void UnloadEntity(uint32_t entityId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->unloadEntity(entId);
}

uint32_t GetEntityChildEntityId(uint32_t entityId, int32_t childId) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId entId;
    entId.setRawId(entityId);
    auto childEntId = EDITOR_APP->getEntityChildEntityId(entId, static_cast<EntityChildId>(childId));
    return childEntId.getRawId();
}

const char* GetEntityName(uint32_t entityId) {
    if(!EDITOR_APP) {
        return nullptr;
    }
    EntityId entId;
    entId.setRawId(entityId);
    return EDITOR_APP->getEntityName(entId);
}

void DrawFrame(void* out, uint32_t w, uint32_t h) {
    if(!EDITOR_APP) {
        return;
    }
    EDITOR_APP->drawFrame(out, w, h);
}

int32_t AddLogicToEntity(uint32_t entityId, const char* logicName) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId entId;
    entId.setRawId(entityId);
    return static_cast<int32_t>(EDITOR_APP->addLogicToEntity(entId, logicName));
}

void RemoveLogicFromEntity(uint32_t entityId, int32_t logicId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->removeLogicFromEntity(entId, static_cast<EntityLogicId>(logicId));
}

int32_t AddChildEntityToEntity(uint32_t parentId, uint32_t childId) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId parentEntId;
    parentEntId.setRawId(parentId);
    EntityId childEntId;
    childEntId.setRawId(childId);
    return  EDITOR_APP->addChilEntityToEntity(parentEntId, childEntId);
}

void RemoveChildEntityFromEntity(uint32_t parentEntityId, uint32_t childEntityId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId parentEntId;
    parentEntId.setRawId(parentEntityId);
    EntityId childEntId;
    childEntId.setRawId(childEntityId);
    EDITOR_APP->removeChildEntityFromEntity(parentEntId, childEntId);
}

void AddEntityLogicArrayElement(uint32_t entityId, int32_t logicId, int32_t valueId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->addEntityLogicArrayElement(entId, static_cast<EntityLogicId>(logicId),
        static_cast<EntityLogicValueId>(valueId));
}

void SetEntityLogicPolymorphObjectType(uint32_t entityId, int32_t logicId, int32_t valueId, const char* newType) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->setEntityLogicPolymorphObjectType(entId, static_cast<EntityLogicId>(logicId),
        static_cast<EntityLogicValueId>(valueId), newType);
}

uint32_t GetEntityLogicData(uint32_t entityId, int32_t logicId, int32_t valueId, void** out) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId entId;
    entId.setRawId(entityId);
    auto buffer = EDITOR_APP->getEntityLogicData(entId, static_cast<EntityLogicId>(logicId),
        static_cast<EntityLogicValueId>(valueId));
    INTERNAL_BUFFER = std::move(buffer);
    *out = INTERNAL_BUFFER.getWriteData();
    return static_cast<uint32_t>(INTERNAL_BUFFER.getSize());
}

void SetEntityLogicData(uint32_t entityId, int32_t logicId, int32_t valueId, const void* data, uint32_t size) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    Buffer buffer(data, size);
    EDITOR_APP->setEntityLogicData(entId, static_cast<EntityLogicId>(logicId),
        static_cast<EntityLogicValueId>(valueId), buffer);
}

void UnloadAll() {
    if(!EDITOR_APP) {
        return;
    }
    EDITOR_APP->unloadAll();
}

int32_t CreateChildEntity(uint32_t entityId, const char* childName) {
    if(!EDITOR_APP) {
        return InvalidEntityChildId;
    }
    EntityId entId;
    entId.setRawId(entityId);
    return EDITOR_APP->createChildEntity(entId, childName);
}

void MouseInputEvent(uint32_t actionType, uint32_t xPos, uint32_t yPos) {
    if(!EDITOR_APP) {
        return;
    }
    Vec2i pos(xPos, yPos);
    EActionType type = static_cast<EActionType>(actionType);
    EDITOR_APP->mouseInputEvent(type, pos);
}

void SetGameTimeScale(float timeScale) {
    if(!EDITOR_APP) {
        return;
    }
    EDITOR_APP->setTimeScale(timeScale);
}

void EnableGameUpdate(bool flag) {
    if(!EDITOR_APP) {
        return;
    }
    EDITOR_APP->enableGameUpdate(flag);
}

int32_t RenameEntity(uint32_t entityId, const char* newName) {
    if(!EDITOR_APP) {
        return -1;
    }
    EntityId entId;
    entId.setRawId(entityId);
    auto res = EDITOR_APP->renameEntity(entId, newName);
    if(res) {
        return 0;
    }
    return -1;
}

void SetFocusEntity(uint32_t entityId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->setFocusEntity(entId);
}