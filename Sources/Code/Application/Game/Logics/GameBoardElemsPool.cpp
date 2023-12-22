#include "Game/Logics/GameBoardElemsPool.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

void GameBoardElemsPool::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardElemsPool>("GameBoardElemsPool")) {
        classInfo->addField("blueEntity", &GameBoardElemsPool::blueEntity);
        classInfo->addField("redEntity", &GameBoardElemsPool::redEntity);
        classInfo->addField("yellowEntity", &GameBoardElemsPool::yellowEntity);
        classInfo->addField("purpleEntity", &GameBoardElemsPool::purpleEntity);
        classInfo->addField("greenEntity", &GameBoardElemsPool::greenEntity);
        classInfo->addField("vRocketEntity", &GameBoardElemsPool::vRocketEntity);
        classInfo->addField("hRocketEntity", &GameBoardElemsPool::hRocketEntity);
        classInfo->addField("bombEntity", &GameBoardElemsPool::bombEntity);
        classInfo->addField("starEntity", &GameBoardElemsPool::starEntity);
    }
}

GameBoardElemsPool::GameBoardElemsPool() {
}

GameBoardElemsPool::~GameBoardElemsPool() {
}

void GameBoardElemsPool::init() {
    ETNode<ETGameBoardElemsPool>::connect(getEntityId());
}

void GameBoardElemsPool::deinit() {
    cleanUpPools();
}

EntityId GameBoardElemsPool::ET_spawnBaseRandomElem() {
    auto elemType = EBoardElemType::None;

    int currWeight = intGenerator.generate();
    for(auto& pool : poolsSetup) {
        if(pool.weight > currWeight) {
            elemType = pool.elemType;
            break;
        } else {
            currWeight -= pool.weight;
        }
    };
    if(elemType == EBoardElemType::None) {
        LogError("[GameBoardElemsPool::ET_spawnBaseRandomElem] Can't select elem pool");
        return InvalidEntityId;
    }
    return ET_spawnElem(elemType);
}

EntityId GameBoardElemsPool::ET_spawnElem(EBoardElemType elemType) {
    auto it = poolsElems.find(elemType);
    if(it == poolsElems.end()) {
        LogError("[GameBoardElemsPool::ET_spawnElem] Can't elems pool for elem type: '%d'", elemType);
        return InvalidEntityId;
    }

    auto& elems = it->second;
    for(auto& elem : elems) {
        if(!elem.isAlive) {
            elem.isAlive = true;
            return elem.entId;
        }
    }

    auto entityRes = getEntityResource(elemType);
    if(!entityRes) {
        LogError("[GameBoardElemsPool::ET_spawnElem] The entity is not set for an elem type: '%d'", elemType);
        return InvalidEntityId;
    }

    ElemInfo newElemInfo;
    newElemInfo.entId = entityRes->createInstance();
    if(!newElemInfo.entId.isValid()) {
        LogError("[GameBoardElemsPool::ET_spawnElem] Can't create elem entity: '%s'", entityRes->getPath());
        return InvalidEntityId;
    }

    newElemInfo.isAlive = true;
    elems.push_back(newElemInfo);

    return newElemInfo.entId;
}

void GameBoardElemsPool::ET_setPoolsSetup(const std::vector<GameElemPoolInfo>& newPoolsSetup) {
    cleanUpPools();
    poolsSetup = newPoolsSetup;
    initPools();
}

void GameBoardElemsPool::initPools() {
    int totalWeight = 0;
    for(auto& pool : poolsSetup) {
        if(pool.weight < 0) {
            LogWarning("[GameBoardElemPool::initPools] Elements of type: '%d' have negative weight: '%d'",
                pool.elemType, pool.weight);
            pool.weight = 0;
        }
        if(GameUtils::IsTriggerType(pool.elemType)) {
            LogWarning("[GameBoardElemPool::initPools] There are special elements of type '%d' in pools setup",
                pool.elemType);
        }
        auto it = poolsElems.find(pool.elemType);
        if(it != poolsElems.end()) {
            LogWarning("[GameBoardElemPool::initPools] Found duplicate elem type '%d' in pools setup",
                pool.elemType);
            continue;
        }
        poolsElems[pool.elemType];
        totalWeight += pool.weight;
    }

    if(totalWeight > 0) {
        intGenerator.setRange(0, totalWeight - 1);
    }

    poolsElems[EBoardElemType::VRocket];
    poolsElems[EBoardElemType::HRocket];
    poolsElems[EBoardElemType::Bomb];
    poolsElems[EBoardElemType::Star];
}

void GameBoardElemsPool::cleanUpPools() {
    for(auto& node : poolsElems) {
        for(auto& elemInfo : node.second) {
            ET_SendEvent(&ETEntityManager::ET_destroyEntity, elemInfo.entId);
        }
    }
    poolsElems.clear();
    poolsSetup.clear();
}

void GameBoardElemsPool::ET_removeElem(EntityId elemId) {
    if(!elemId.isValid()) {
        LogError("[GameBoardElemsPool::ET_removeElem] Can't remove invalid elem");
        return;
    }

    auto elemType = GameUtils::GetElemType(elemId);
    auto it = poolsElems.find(elemType);
    if(it == poolsElems.end()) {
        LogError("[GameBoardElemsPool::ET_removeElem] Can't find pool for elemType: '%d' of elem: '%s'",
            elemType, EntityUtils::GetEntityName(elemId));
        return;
    }

    for(auto& elemInfo : it->second) {
        if(elemInfo.entId == elemId) {
            elemInfo.isAlive = false;
            ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, elemInfo.entId);
            return;
        }
    }

    LogError("[GameBoardElemsPool::ET_removeElem] Can't find spawned elem: '%s'",
        EntityUtils::GetEntityName(elemId));
}

const EntityResource* GameBoardElemsPool::getEntityResource(EBoardElemType elemType) const {
    switch(elemType) {
        case EBoardElemType::Red: {
            return &redEntity;
        }
        case EBoardElemType::Blue: {
            return &blueEntity;
        }
        case EBoardElemType::Green: {
            return &greenEntity;
        }
        case EBoardElemType::Purple: {
            return &purpleEntity;
        }
        case EBoardElemType::Yellow: {
            return &yellowEntity;
        }
        case EBoardElemType::HRocket: {
            return &hRocketEntity;
        }
        case EBoardElemType::VRocket: {
            return &vRocketEntity;
        }
        case EBoardElemType::Bomb: {
            return &bombEntity;
        }
        case EBoardElemType::Star: {
            return &starEntity;
        }
        case EBoardElemType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid elem type");
        }
    }
    return nullptr;
}

void GameBoardElemsPool::ET_setElemEntity(EBoardElemType elemType, const char* entityName) {
    switch(elemType) {
        case EBoardElemType::Red: {
            redEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Blue: {
            blueEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Green: {
            greenEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Purple: {
            purpleEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Yellow: {
            yellowEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::HRocket: {
            hRocketEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::VRocket: {
            vRocketEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Bomb: {
            bombEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::Star: {
            starEntity.setPath(entityName);
            break;
        }
        case EBoardElemType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid elem type");
        }
    }
}