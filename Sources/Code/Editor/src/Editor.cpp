#include "Editor.hpp"
#include "EditorApp.hpp"

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