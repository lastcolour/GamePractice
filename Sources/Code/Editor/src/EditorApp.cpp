#include "EditorApp.hpp"
#include "Render/RenderModule.hpp"
#include "Audio/AudioModule.hpp"
#include "Game/GameModule.hpp"
#include "UI/UIModule.hpp"
#include "Entity/EntityModule.hpp"
#include "Reflect/ETReflectInterfaces.hpp"
#include "Core/ETPrimitives.hpp"
#include "Platform/PlatformModule.hpp"
#include "EditorModule.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Entity/ETEntity.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/MemoryStream.hpp"
#include "Core/ETAssets.hpp"
#include "Core/ETLogger.hpp"
#include "ETEditorInterfaces.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Core/ETTasks.hpp"
#include "UI/ETUITimer.hpp"
#include "Game/ETGameTimer.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Core/GlobalEnvironment.hpp"

namespace {

const int EDITOR_THREADS_COUNT = 3;

} // namespace

EditorApp::EditorApp() :
    Application(),
    updateGame(false) {
}

EditorApp::~EditorApp() {
}

bool EditorApp::initialize() {
    if(!init()) {
        return false;
    }

    buildTasksRunner();
    GetEnv()->GetTasksRunner()->startOtherThreads(EDITOR_THREADS_COUNT);

    enableGameUpdate(false);

    return true;
}

void EditorApp::deinitiazlie() {
    GetEnv()->GetTasksRunner()->stopOtherTreads();
    deinit();
}

void EditorApp::buildTasksRunner() {
    auto taskRunner = GetEnv()->GetTasksRunner();
    {
        auto assetsUpdate = taskRunner->createTask("Assets", [](float dt){
            ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets, dt);
        });
        assetsUpdate->setType(RunTaskType::NoInMainThread);
        assetsUpdate->setFrequency(120);
    }
    {
        auto entitiesUpdate = taskRunner->createTask("Entities", [](){
            ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
        });
        entitiesUpdate->setFrequency(120);
    }
    {
        auto uiUpdate = taskRunner->createTask("UI", [](float dt){
            ET_SendEvent(&ETUITimer::ET_onTick, dt);
        });
        auto renderSync = taskRunner->createTask("RenderSync", [](){
            ET_SendEvent(&ETRenderUpdateTask::ET_syncWithGame);
        });
        renderSync->setType(RunTaskType::MainThreadOnly);
        auto gameUpdate = taskRunner->createTask("Game", [](float dt){
            ET_SendEvent(&ETGameTimer::ET_onTick, dt);
        });
        gameUpdate->setFrequency(120);
        gameUpdate->addChild(uiUpdate);
        uiUpdate->addChild(renderSync);
    }
    {
        auto updateParticles = taskRunner->createTask("UpdateParticles", [](float dt){
            ET_SendEvent(&ETRenderUpdateTask::ET_updateParticles, dt);
        });
        updateParticles->setType(RunTaskType::MainThreadOnly);
        updateParticles->setFrequency(60);
    }
    {
        auto soundUpdate = taskRunner->createTask("Sound", [](){
            ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
        });
        soundUpdate->setFrequency(60);
    }
}

Buffer EditorApp::getReflectModel() {
    JSONNode node;
    ET_SendEvent(&ETClassInfoManager::ET_makeReflectModel, node);
    if(!node) {
        return Buffer();
    }
    return node.flushToBuffer();
}

Buffer EditorApp::getRegisteredEntityLogics() {
    JSONNode node;
    ET_SendEventReturn(node, &ETEntityManager::ET_getRegisteredLogics);
    if(!node) {
        return Buffer();
    }
    return node.flushToBuffer();
}

void EditorApp::buildModules(ModuleListT& modules) {
    modules.emplace_back(new EditorModule);
    modules.emplace_back(new PlatformModule);
    modules.emplace_back(new EntityModule);
    modules.emplace_back(new AudioModule);
    modules.emplace_back(new RenderModule);
    modules.emplace_back(new UIModule);
    modules.emplace_back(new GameModule);
}

EntityId EditorApp::loadEntityFromFile(const char* entityName) {
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, entityName);
    if(!entId.isValid()) {
        return InvalidEntityId;
    }
    ET_SendEvent(&ETEditEntityTracker::ET_startTrackingEntity, entId);
    return entId;
}

EntityId EditorApp::loadEntityFromData(const char* entityName, const char* entityData) {
    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntityFromData, entityName, entityData);
    return entId;
}

void EditorApp::unloadEntity(EntityId entityId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::unloadEntity] Can't unload entity with invalid id");
        return;
    }
    ET_SendEvent(&ETEditEntityTracker::ET_stopTrackingEntity, entityId);
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, entityId);
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
}

EntityId EditorApp::getEntityChildEntityId(EntityId entityId, EntityChildId childId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityChildren] Can't get children of invalid entity");
        return InvalidEntityId;
    }
    EntityId childEntityId;
    ET_SendEventReturn(childEntityId, entityId, &ETEntity::ET_getEntityIdFromChildId, childId);
    return childEntityId;
}

const char* EditorApp::getEntityName(EntityId entityId) {
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityName] Can't get name of invalid entity");
        return nullptr;
    }
    return EntityUtils::GetEntityName(entityId);
}

void EditorApp::drawFrame(void* out, int32_t width, int32_t height) {
    Vec2i viewport = Vec2i(width, height);
    ET_SendEvent(&ETRenderCamera::ET_setRenderPort, viewport);

    GetEnv()->GetTasksRunner()->stepMainTread();

    ET_SendEvent(&ETRender::ET_drawFrameToBuffer, imageBuffer, Vec2i(width, height), DrawContentFilter::None);
    memcpy(out, imageBuffer.getData().getReadData(), viewport.x * viewport.y * 4);
}

EntityLogicId EditorApp::addLogicToEntity(EntityId entityId, const char* logicName) {
    EntityLogicId logicId = InvalidEntityLogicId;
    ET_SendEventReturn(logicId, &ETEntityManager::ET_addLogicToEntity, entityId, logicName);
    return static_cast<int32_t>(logicId);
}

void EditorApp::removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) {
    ET_SendEvent(&ETEntityManager::ET_removeLogicFromEntity, entityId, logicId);
}

EntityChildId EditorApp::addChilEntityToEntity(EntityId parentEntId, EntityId childEntId) {
    if(!parentEntId.isValid()) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add child entity to parent entity with invalid id");
        return InvalidEntityChildId;
    }
    if(!ET_IsExistNode<ETEntity>(parentEntId)) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add child entity to parent entity that does not exist");
        return InvalidEntityChildId;
    }
    if(!childEntId.isValid()) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add child entity with invalid id");
        return InvalidEntityChildId;
    }
    if(!ET_IsExistNode<ETEntity>(childEntId)) {
        LogError("[EditorApp::addChilEntityToEntity] Can't add child entity that does not exist");
        return InvalidEntityChildId;
    }
    EntityChildId childId = InvalidEntityChildId;
    ET_SendEventReturn(childId, parentEntId, &ETEntity::ET_addChild, childEntId);
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
    MemoryStream stream;
    stream.openForWrite();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entityId, logicId, valueId, stream);
    if(!res) {
        return Buffer();
    }
    return stream.flushToBuffer();
}

void EditorApp::setEntityLogicData(EntityId entityId, EntityLogicId logicId, EntityLogicValueId valueId, Buffer& buffer) {
    MemoryStream stream;
    stream.openForRead(buffer);
    ET_SendEvent(&ETEntityManager::ET_writeEntityLogicData, entityId, logicId, valueId, stream);
}

void EditorApp::addEntityLogicArrayElement(EntityId entityId, EntityLogicValueId logicId, EntityLogicValueId valueId) {
    ET_SendEvent(&ETEntityManager::ET_addEntityLogicArrayElement, entityId, logicId, valueId);
}

EntityChildId EditorApp::createChildEntity(EntityId entityId, const char* childName) {
    EntityChildId childId = InvalidEntityChildId;
    ET_SendEventReturn(childId, &ETEntityManager::ET_createChildEntity, entityId, childName);
    return childId;
}

void EditorApp::mouseInputEvent(EActionType actionType, const Vec2i& pos) {
    ET_QueueEvent(&ETInputEvents::ET_onTouch, actionType, pos);
}

void EditorApp::unloadAll() {
    ET_SendEvent(&ETEntityManager::ET_destroyAllEntities);
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
}

void EditorApp::setTimeScale(float timeScale) {
    ET_SendEvent(&ETUITimer::ET_setScale, timeScale);
    ET_SendEvent(&ETGameTimer::ET_setScale, timeScale);
}

void EditorApp::enableGameUpdate(bool flag) {
    if(!flag) {
        ET_QueueEvent(&ETUITimer::ET_pause);
        ET_QueueEvent(&ETGameTimer::ET_pause);
        ET_QueueEvent(&ETAudioSystem::ET_setMasterVolume, 0.f);
    } else {
        ET_QueueEvent(&ETUITimer::ET_resume);
        ET_QueueEvent(&ETGameTimer::ET_resume);
        ET_QueueEvent(&ETAudioSystem::ET_setMasterVolume, 1.f);
    }
}

bool EditorApp::renameEntity(EntityId entityId, const char* newName) {
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_renameEntity, entityId, newName);
    return res;
}