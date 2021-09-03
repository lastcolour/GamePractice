#include "Game/Logics/GameBoardEffects.hpp"
#include "Game/ETGameElem.hpp"
#include "Render/ETParticlesSystem.hpp"

#include <cassert>

namespace {

void spawnEffectWithScale(EntityId targetEntId, EntityId effectId, float scale) {
    if(!targetEntId.isValid()) {
        return;
    }
    if(!effectId.isValid()) {
        return;
    }

    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    Transform tm;
    ET_SendEventReturn(tm, targetEntId, &ETEntity::ET_getTransform);
    tm.scale = Vec3(static_cast<float>(cellSize) * scale);

    ET_SendEvent(effectId, &ETParticlesSystem::ET_emitWithTm, tm);
}

} // namespace

void GameBoardEffects::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardEffects>("GameBoardEffects")) {
        classInfo->addField("redDestroy", &GameBoardEffects::redDestroyEffectId);
        classInfo->addField("blueDestroy", &GameBoardEffects::blueDestroyEffectId);
        classInfo->addField("yellowDestroy", &GameBoardEffects::yellowDestroyEffectId);
        classInfo->addField("greenDestroy", &GameBoardEffects::greenDestroyEffectId);
        classInfo->addField("purpleDestroy", &GameBoardEffects::purpleDestroyEffectId);
        classInfo->addField("bomdDestroy", &GameBoardEffects::bomdDestroyEffectId);
        classInfo->addField("rocketTrail", &GameBoardEffects::rocketTrailEffectId);
        classInfo->addField("rocketDestroy", &GameBoardEffects::rocketDestroyEffectId);
        classInfo->addField("mutateEffect", &GameBoardEffects::mutateEffectId);
        classInfo->addField("mergeEffect", &GameBoardEffects::mergeEffectId);
        classInfo->addField("destroyEffectScale", &GameBoardEffects::destroyEffectScale);
    }
}

GameBoardEffects::GameBoardEffects() :
    destroyEffectScale(1.f) {
}

GameBoardEffects::~GameBoardEffects() {
}

void GameBoardEffects::init() {
    ETNode<ETGameBoardEffects>::connect(getEntityId());
}

void GameBoardEffects::deinit() {
}

EntityId GameBoardEffects::ET_getRocketTrailEffect() const {
    return rocketTrailEffectId;
}

void GameBoardEffects::ET_playDestroyEffect(EntityId elemId) {
    EBoardElemType elemType = EBoardElemType::None;
    ET_SendEventReturn(elemType, elemId, &ETGameBoardElem::ET_getType);

    EntityId effectId;
    switch(elemType) {
        case EBoardElemType::Red: {
            effectId = redDestroyEffectId;
            break;
        }
        case EBoardElemType::Blue: {
            effectId = blueDestroyEffectId;
            break;
        }
        case EBoardElemType::Yellow: {
            effectId = yellowDestroyEffectId;
            break;
        }
        case EBoardElemType::Purple: {
            effectId = purpleDestroyEffectId;
            break;
        }
        case EBoardElemType::Green: {
            effectId = greenDestroyEffectId;
            break;
        }
        case EBoardElemType::VRocket: {
            effectId = rocketDestroyEffectId;
            break;
        }
        case EBoardElemType::HRocket: {
            effectId = rocketDestroyEffectId;
            break;
        }
        case EBoardElemType::Bomb: {
            effectId = bomdDestroyEffectId;
            break;
        }
        case EBoardElemType::Star: {
            break;
        }
        case EBoardElemType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid elem type");
        }
    }

    spawnEffectWithScale(elemId, effectId, destroyEffectScale);
}

void GameBoardEffects::ET_playMergeEffect(EntityId elemId) {
    spawnEffectWithScale(elemId, mergeEffectId, destroyEffectScale);
}

void GameBoardEffects::ET_playMutateEffect(EntityId elemId) {
    spawnEffectWithScale(elemId, mutateEffectId, destroyEffectScale);
}
