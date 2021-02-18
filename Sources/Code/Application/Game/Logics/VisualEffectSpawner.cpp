#include "Game/Logics/VisualEffectSpawner.hpp"
#include "Render/ETPatricle.hpp"
#include "Entity/ETEntity.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Core/ETLogger.hpp"

void VisualEffectSpawner::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<VisualEffectSpawner>("VisualEffectSpawner")) {
        classInfo->addResourceField("effectEntity", ResourceType::Entity, &VisualEffectSpawner::effectEntityName);
    }
}

VisualEffectSpawner::VisualEffectSpawner() {
}

VisualEffectSpawner::~VisualEffectSpawner() {
}

void VisualEffectSpawner::init() {
    ETNode<ETVisualEffectSpawner>::connect(getEntityId());
}

void VisualEffectSpawner::deinit() {
}

void VisualEffectSpawner::ET_spawn(const VisualEffectSpawnConfig& spawnConfig) {
    EntityId effectId;
    ET_SendEventReturn(effectId, &ETEntityManager::ET_createEntity, effectEntityName.c_str());
    if(!effectId.isValid()) {
        LogError("[VisualEffectSpawner::ET_spawn] Can't spawn effect entity: '%s'", effectEntityName);
        return;
    }
    ET_SendEvent(effectId, &ETEntity::ET_setTransform, spawnConfig.tm);
    ET_SendEvent(effectId, &ETParticleEmitter::ET_emit);
    activeEffects.push_back(effectId);
}