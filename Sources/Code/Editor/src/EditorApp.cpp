#include "EditorApp.hpp"
#include "Render/RenderModule.hpp"
#include "Audio/AudioModule.hpp"
#include "Game/GameModule.hpp"
#include "UI/UIModule.hpp"
#include "CoreModule.hpp"
#include "Entity/EntityModule.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "Core/JSONNode.hpp"
#include "Core/Buffer.hpp"
#include "Platforms/PlatformModule.hpp"
#include "EditorModule.hpp"
#include "Entity/ETEntityInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"

EditorApp::EditorApp() :
    Application() {
}

EditorApp::~EditorApp() {
}

bool EditorApp::initialize() {
    return init();
}

void EditorApp::deinitiazlie() {
    deinit();
}

const char* EditorApp::getReflectModel() {
    if(!reflectModelBuffer) {
        JSONNode node;
        ET_SendEvent(&ETClassInfoManager::ET_makeReflectModel, node);
        if(!node) {
            return nullptr;
        }
        auto buffer = node.flushToBuffer();
        reflectModelBuffer = std::move(buffer);
    }
    return reflectModelBuffer.getCString();
}

void EditorApp::buildModules(ModuleListT& modules) {
    modules.emplace_back(new CoreModule);
    modules.emplace_back(new EditorModule);
    modules.emplace_back(new PlatformModule);
    modules.emplace_back(new EntityModule);
    modules.emplace_back(new AudioModule);
    modules.emplace_back(new RenderModule);
    modules.emplace_back(new UIModule);
    modules.emplace_back(new GameModule);
}

EntityId EditorApp::loadEntity(const char* entityName) {
    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, entityName);
    if(!entId.isValid()) {
        return entId;
    }
    if(centralEntityId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, centralEntityId);
    }
    centralEntityId = entId;
    return centralEntityId;
}

void EditorApp::unloadEntity(EntityId entityId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::unloadEntity] Can't unload entity with invalid id");
        return;
    }
    if(entityId != centralEntityId) {
        LogError("[EditorApp::unloadEntity] Can't unload non-central entity");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, centralEntityId);
    centralEntityId = InvalidEntityId;
}