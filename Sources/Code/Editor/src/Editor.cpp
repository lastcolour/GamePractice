#include "Editor.hpp"
#include "EditorApp.hpp"
#include "Core/Core.hpp"
#include "Core/MemoryStream.hpp"
#include "Core/Buffer.hpp"

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

const char* GetReflectModel() {
    if(!EDITOR_APP) {
        return nullptr;
    }
    return EDITOR_APP->getReflectModel();
}

void DeInitialize() {
    if(!EDITOR_APP) {
        return;
    }
    delete EDITOR_APP;
    EDITOR_APP = nullptr;
}

uint32_t LoadEntity(const char* entityName) {
    if(!EDITOR_APP) {
        return InvalidEntityId.getRawId();
    }
    auto entityId = EDITOR_APP->loadEntity(entityName);
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

uint32_t GetEntityChildren(uint32_t entityId, uint32_t* out) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId entId;
    entId.setRawId(entityId);
    auto children = EDITOR_APP->getEntityChildren(entId);
    if(children.empty()) {
        return 0u;
    }
    INTERNAL_BUFFER.resize(children.size() * sizeof(uint32_t));
    auto ptr = static_cast<uint32_t*>(INTERNAL_BUFFER.getWriteData());
    for(uint32_t i = 0u; i < children.size(); ++i) {
        ptr[i] = children[i].getRawId();
    }
    out = ptr;
    return children.size();
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

uint32_t AddLogicToEntity(uint32_t entityId, const char* logicName) {
    if(!EDITOR_APP) {
        return 0u;
    }
    EntityId entId;
    entId.setRawId(entityId);
    return EDITOR_APP->addLogicToEntity(entId, logicName);
}

void RemoveLogicFromEntity(uint32_t entityId, uint32_t logicId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->removeLogicFromEntity(entId, logicId);
}

uint32_t GetEntityLogicData(uint32_t entityId, uint32_t logicId, void* out) {
    if(!EDITOR_APP) {
        return 0u;
    }
}

void SetEntityLogicFieldData(uint32_t entityId, uint32_t logicId, uint32_t fieldId, void* data, uint32_t size) {
    if(!EDITOR_APP) {
        return;
    }
}