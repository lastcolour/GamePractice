#include "EditorApp.hpp"
#include "Render/RenderModule.hpp"
#include "Audio/AudioModule.hpp"
#include "Game/GameModule.hpp"
#include "UI/UIModule.hpp"
#include "Entity/EntityModule.hpp"
#include "Reflect/ClassInfoManager.hpp"
#include "Platform/PlatformModule.hpp"
#include "EditorModule.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Core/MemoryStream.hpp"
#include "Core/ETAssets.hpp"
#include "ETEditorInterfaces.hpp"
#include "Parallel/TasksRunner.hpp"
#include "Core/ETTasks.hpp"
#include "UI/ETUITimer.hpp"
#include "Game/ETGameTimer.hpp"
#include "Audio/ETAudioSystem.hpp"
#include "Render/ETRenderCamera.hpp"

namespace {

const int EDITOR_THREADS_COUNT = 2;
const int EDITOR_MAX_GAME_TICK_RATE = 60;
const int EDITOR_AUX_TASK_TICK_RATE = EDITOR_MAX_GAME_TICK_RATE * 2;

} // namespace

EditorApp::EditorApp() :
    Application(),
    tickCount(0),
    asyncTickingTasks(0),
    canTickAsync(0),
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
    auto runner = GetEnv()->GetTasksRunner();
    assert(runner && "Invalid task runner");
    {
        auto assetsUpdate = runner->createTask("Assets", [](float dt){
            ET_SendEvent(&ETAssetsUpdateTask::ET_updateAssets, dt);
        });
        assetsUpdate->setType(RunTaskType::NoInMainThread);
        assetsUpdate->setFrequency(EDITOR_AUX_TASK_TICK_RATE);
    }
    {
        auto entitiesUpdate = runner->createTask("Entities", [](){
            ET_SendEvent(&ETEntitiesUpdateTask::ET_updateEntities);
        });
        entitiesUpdate->setType(RunTaskType::NoInMainThread);
        entitiesUpdate->setFrequency(EDITOR_AUX_TASK_TICK_RATE);
    }
    {
        auto soundUpdate = runner->createTask("Sound", [](){
            ET_SendEvent(&ETSoundUpdateTask::ET_updateSound);
        });
        soundUpdate->setFrequency(EDITOR_AUX_TASK_TICK_RATE);
    }

    auto uiUpdate = runner->createTask("UI", [this](float dt){
        if(canTickAsync.load()) {
            ++asyncTickingTasks;
            ET_SendEvent(&ETUITimer::ET_onTick, dt);
            --asyncTickingTasks;
        }
    });
    uiUpdate->setFrequency(EDITOR_MAX_GAME_TICK_RATE);

    auto gameUpdate = runner->createTask("Game", [this](float dt){
        if(canTickAsync.load()) {
            ++asyncTickingTasks;
            ET_SendEvent(&ETGameTimer::ET_onTick, dt);
            --asyncTickingTasks;
        }
    });
    gameUpdate->setFrequency(EDITOR_MAX_GAME_TICK_RATE);

    auto preRender = runner->createTask("PreRender", [this](){
        ET_SendEvent(&ETRenderUpdateTask::ET_PreRender);
        ++tickCount;
    });
    preRender->setFrequency(EDITOR_MAX_GAME_TICK_RATE);
    preRender->setType(RunTaskType::MainThreadOnly);

    gameUpdate->addChild(uiUpdate);
    gameUpdate->addChild(preRender);

    uiUpdate->addChild(preRender);
}

Memory::Buffer EditorApp::getReflectModel() {
    JSONNode node;
    GetEnv()->GetClassInfoManager()->makeReflectModel(node);
    if(!node) {
        return Memory::Buffer();
    }
    return node.flushToBuffer();
}

Memory::Buffer EditorApp::getRegisteredEntityLogics() {
    JSONNode node;
    ET_SendEventReturn(node, &ETEntityManager::ET_getRegisteredLogics);
    if(!node) {
        return Memory::Buffer();
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
    AsyncTickLock aLock(*this);
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntity, entityName);
    if(!entId.isValid()) {
        return InvalidEntityId;
    }
    Vec2i renderPort(0);
    ET_SendEventReturn(renderPort, &ETRenderCamera::ET_getRenderPort);

    Transform tm;
    tm.pt.x = renderPort.x / 2.f;
    tm.pt.y = renderPort.y / 2.f;
    ET_SendEvent(entId, &ETEntity::ET_setTransform, tm);
    return entId;
}

EntityId EditorApp::loadEntityFromData(const char* entityName, const char* entityData) {
    AsyncTickLock aLock(*this);
    EntityId entId;
    ET_SendEventReturn(entId, &ETEntityManager::ET_createEntityFromData, entityName, entityData);
    return entId;
}

void EditorApp::unloadEntity(EntityId entityId) {
    AsyncTickLock aLock(*this);
    if(!entityId.isValid()) {
        LogError("[EditorApp::unloadEntity] Can't unload entity with invalid id");
        return;
    }
    ET_SendEvent(&ETEntityManager::ET_destroyEntity, entityId);
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
}

EntityId EditorApp::getEntityChildEntityId(EntityId entityId, EntityChildId childId) {
    AsyncTickLock aLock(*this);
    if(!entityId.isValid()) {
        LogError("[EditorApp::getEntityChildren] Can't get children of invalid entity");
        return InvalidEntityId;
    }
    EntityId childEntityId;
    std::vector<EntityChildId> childrenIds(1, childId);
    ET_SendEventReturn(childEntityId, entityId, &ETEntity::ET_getEntityIdFromChildId, childrenIds);
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
    ET_SendEvent(&ETSurfaceEvents::ET_onSurfaceResized, Vec2i(width, height));

    const int prevTickCount = tickCount.load();
    while(true) {
        GetEnv()->GetTasksRunner()->stepMainThread();
        if(prevTickCount != tickCount.load()) {
            break;
        }
    }

    ET_SendEvent(&ETRender::ET_drawFrameToBufferRaw, out, Vec2i(width, height), EDrawContentFilter::None);
}

EntityLogicId EditorApp::addLogicToEntity(EntityId entityId, const char* logicName) {
    AsyncTickLock aLock(*this);
    EntityLogicId logicId = InvalidEntityLogicId;
    ET_SendEventReturn(logicId, &ETEntityManager::ET_addLogicToEntity, entityId, logicName);
    return static_cast<int32_t>(logicId);
}

void EditorApp::removeLogicFromEntity(EntityId entityId, EntityLogicId logicId) {
    AsyncTickLock aLock(*this);
    ET_SendEvent(&ETEntityManager::ET_removeLogicFromEntity, entityId, logicId);
}

EntityChildId EditorApp::addChilEntityToEntity(EntityId parentEntId, EntityId childEntId) {
    AsyncTickLock aLock(*this);
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
    AsyncTickLock aLock(*this);
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

Memory::Buffer EditorApp::getEntityLogicData(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId) {
    AsyncTickLock aLock(*this);
    Memory::MemoryStream stream;
    stream.openForWrite();
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_readEntityLogicData, entityId, logicId, valueId, stream);
    if(!res) {
        return Memory::Buffer();
    }
    return stream.flushToBuffer();
}

void EditorApp::setEntityLogicData(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId, Memory::Buffer& buffer) {
    AsyncTickLock aLock(*this);
    Memory::MemoryStream stream;
    stream.openForRead(buffer);
    ET_SendEvent(&ETEntityManager::ET_writeEntityLogicData, entityId, logicId, valueId, stream);
}

void EditorApp::addEntityLogicArrayElement(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId) {
    AsyncTickLock aLock(*this);
    ET_SendEvent(&ETEntityManager::ET_addEntityLogicArrayElement, entityId, logicId, valueId);
}

void EditorApp::setEntityLogicPolymorphObjectType(EntityId entityId, EntityLogicId logicId, Reflect::ClassValueId valueId, const char* newType) {
    AsyncTickLock aLock(*this);
    ET_SendEvent(&ETEntityManager::ET_setEntityLogicPolymorphObjectType, entityId, logicId, valueId, newType);
}

EntityChildId EditorApp::createChildEntity(EntityId entityId, const char* childName) {
    AsyncTickLock aLock(*this);
    EntityChildId childId = InvalidEntityChildId;
    ET_SendEventReturn(childId, &ETEntityManager::ET_createChildEntity, entityId, childName);
    return childId;
}

void EditorApp::mouseInputEvent(EActionType actionType, const Vec2i& pos) {
    TouchEvent event;
    event.actionType = actionType;
    event.eventT = TimePoint::GetNow();
    event.pt = pos;
    ET_QueueEvent(&ETInputEvents::ET_onTouch, event);
}

void EditorApp::unloadAll() {
    AsyncTickLock aLock(*this);
    ET_SendEvent(&ETEntityManager::ET_destroyAllEntities);
    ET_SendEvent(&ETAssetsCacheManager::ET_clear);
}

void EditorApp::setTimeScale(float timeScale) {
    AsyncTickLock aLock(*this);
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
    AsyncTickLock aLock(*this);
    bool res = false;
    ET_SendEventReturn(res, &ETEntityManager::ET_renameEntity, entityId, newName);
    return res;
}

void EditorApp::setFocusEntity(EntityId entityId) {
    ET_SendEvent(&ETEntityEditorHelper::ET_setFocusEntity, entityId);
}