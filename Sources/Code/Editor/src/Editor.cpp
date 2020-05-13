#include "Editor.hpp"
#include "EditorApp.hpp"
#include "Core/Core.hpp"

EditorApp* EDITOR_APP = nullptr;

int Initiliaze() {
    if(EDITOR_APP) {
        return 1;
    }
    EDITOR_APP = new EditorApp();
    if(!EDITOR_APP) {
        return 2;
    }
    if(!EDITOR_APP->initialize()) {
        delete EDITOR_APP;
        EDITOR_APP = nullptr;
        return 3;
    }
    return 0;
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

int32_t LoadEntity(const char* entityName) {
    if(!EDITOR_APP) {
        return InvalidEntityId.getRawId();
    }
    auto entityId = EDITOR_APP->loadEntity(entityName);
    return entityId.getRawId();
}

void UnloadEntity(int32_t entityId) {
    if(!EDITOR_APP) {
        return;
    }
    EntityId entId;
    entId.setRawId(entityId);
    EDITOR_APP->unloadEntity(entId);
}