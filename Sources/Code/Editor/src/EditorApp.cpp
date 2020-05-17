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
#include "Render/ETRenderInterfaces.hpp"

EditorApp::EditorApp() :
    Application() {
}

EditorApp::~EditorApp() {
}

bool EditorApp::initialize() {
    bool res = init();
    if(!res) {
        return res;
    }

    return res;
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

std::vector<EntityId> EditorApp::getEntityChildren(EntityId entityId) {
    std::vector<EntityId> children;
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityChildren] Can't get children of invalid entity");
        return children;
    }
    ET_SendEventReturn(children, entityId, &ETEntity::ET_getChildren);
    return children;
}

const char* EditorApp::getEntityName(EntityId entityId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityName] Can't get name of invalid entity");
        return nullptr;
    }
    const char* entityName = nullptr;
    ET_SendEventReturn(entityName, entityId, &ETEntity::ET_getName);
    return entityName;
}

void EditorApp::drawFrame(void* out, int32_t width, int32_t height) {
    Vec2i renderSize = Vec2i(width, height);
    frameBuffer.setSize(renderSize);
    frameBuffer.clear();
    ET_SendEvent(&ETRender::ET_drawFrameToFramebufer, frameBuffer);
    memcpy(out, frameBuffer.getPtr(), renderSize.x * renderSize.y * 4);
}

uint32_t EditorApp::addLogicToEntity(EntityId entityId, const char* logicName) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::addLogicToEntity] Can't add logic '%s' to invalid entity", logicName);
        return 0;
    }
    return 0;
}

void EditorApp::removeLogicFromEntity(EntityId entityId, uint32_t logicId) {
}