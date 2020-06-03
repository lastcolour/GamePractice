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
#include "Core/MemoryStream.hpp"

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

std::vector<const char*> EditorApp::getRegisteredEntityLogics() {
    std::vector<const char*> res;
    ET_SendEvent(&ETEntityManager::ET_getRegisteredLogics, res);
    return res;
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

EntityLogicId EditorApp::addLogicToEntity(EntityId entityId, const char* logicName) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::addLogicToEntity] Can't add logic '%s' to invalid entity", logicName);
        return 0;
    }
    EntityLogicId logicId = InvalidEntityLogicId;
    ET_SendEventReturn(logicId, &ETEntityManager::ET_addLogicToEntity, entityId, logicName);
    return static_cast<int32_t>(logicId);
}

void EditorApp::removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::removeLogicFromEntity] Can't remove logic from invalid entity");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_removeLogicFromEntity, entityId, logicId);
}

EntityId EditorApp::addChilEntityToEntity(EntityId entityId, const char* childName) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add entity to invalid entity");
        return InvalidEntityId;
    }
    if(!ET_IsExistNode<ETEntity>(entityId)) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add child entity to entity that does not exist");
        return InvalidEntityId;
    }
    if(!childName || !childName[0]) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add entity with invalid name");
        return InvalidEntityId;
    }
    EntityId childId;
    ET_SendEventReturn(childId, &ETEntityManager::ET_createEntity, childName);
    if(!childId.isValid()) {
        LogError("[EditorApp::addChilEntityToEntity] Can't create entity to add");
        return InvalidEntityId;
    }
    ET_SendEvent(entityId, &ETEntity::ET_addChild, childId);
    return childId;
}

void EditorApp::removeChildEntityFromEntity(EntityId parentId, EntityId childId) {
    if(!parentId.isValid()) {
        LogError("[EditorApp::removeChildEntityFromEntity] Can't remove entity from invalid parent entity");
        return;
    }
    if(!ET_IsExistNode<ETEntity>(parentId)) {
        LogError("[EditorApp::removeChildEntityFromEntity] Can't remove entity from non-exist parent entity");
        return;
    }
    if(!childId.isValid()) {
        LogError("[EditorApp::removeChildEntityFromEntity] Can't remove invalid child entity");
        return;
    }
    if(!ET_IsExistNode<ETEntity>(childId)) {
        LogError("[EditorApp::removeChildEntityFromEntity] Can't remove child that does not exist");
        return;
    }
    ET_SendEvent(parentId, &ETEntity::ET_removeChild, childId);
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, childId);
}

Buffer EditorApp::getEntityLogicData(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityLogicData] Can't get logic data from invalid entity");
        return Buffer();
    }
    MemoryStream stream;
    stream.openForWrite();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entityId, logicId, valueId, stream);
    if(!res) {
        LogError("[EditorApp::getEntityLogicData] Can't read logic value data from entity");
        return Buffer();
    }
    return stream.flushToBuffer();
}

void EditorApp::setEntityLogicData(EntityId entityId, EntityLogicId logicId, EntityLogicValueId valueId, Buffer& buffer) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::setEntityLogicData] Can't set logic value data to invalid entity");
        return;
    }
    MemoryStream stream;
    stream.openForRead(buffer);
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_writeEntityLogicData, entityId, logicId, valueId, stream);
    if(!res) {
        LogError("[EditorApp::setEntityLogicData] Can't write logic value data to entity");
    }
}