#include "Game/Logics/GameBoardElemsPool.hpp"
#include "Entity/ETEntityManager.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

#include <cassert>

namespace {

bool isSpecialElemType(EBoardElemType elemType) {
    bool res = false;
    switch(elemType) {
        case EBoardElemType::HRocket: {
            res = true;
            break;
        }
        case EBoardElemType::VRocket: {
            res = true;
            break;
        }
        case EBoardElemType::Bomb: {
            res = true;
            break;
        }
        case EBoardElemType::Star: {
            res = true;
            break;
        }
        default: {
            res = false;
        }
    }
    return res;
}

} // namespace

void GameBoardElemsPool::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardElemsPool>("GameBoardElemsPool")) {
        classInfo->addResourceField("blueEntity", ResourceType::Entity,
            &GameBoardElemsPool::blueEntity);
        classInfo->addResourceField("redEntity", ResourceType::Entity,
            &GameBoardElemsPool::redEntity);
        classInfo->addResourceField("yellowEntity", ResourceType::Entity,
            &GameBoardElemsPool::yellowEntity);
        classInfo->addResourceField("purpleEntity", ResourceType::Entity,
            &GameBoardElemsPool::purpleEntity);
        classInfo->addResourceField("greenEntity", ResourceType::Entity,
            &GameBoardElemsPool::greenEntity);
        classInfo->addResourceField("vRocketEntity", ResourceType::Entity,
            &GameBoardElemsPool::vRocketEntity);
        classInfo->addResourceField("hRocketEntity", ResourceType::Entity,
            &GameBoardElemsPool::hRocketEntity);
        classInfo->addResourceField("bombEntity", ResourceType::Entity,
            &GameBoardElemsPool::bombEntity);
        classInfo->addResourceField("starEntity", ResourceType::Entity,
            &GameBoardElemsPool::starEntity);
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

    auto entityName = getElemEntityName(elemType);
    if(!entityName || !entityName[0]) {
        LogError("[GameBoardElemsPool::ET_spawnElem] The entity is not set for an elem type: '%d'", elemType);
        return InvalidEntityId;
    }

    ElemInfo newElemInfo;
    ET_SendEventReturn(newElemInfo.entId, &ETEntityManager::ET_createEntity, entityName);
    if(!newElemInfo.entId.isValid()) {
        LogError("[GameBoardElemsPool::ET_spawnElem] Can't create elem entity: '%s'", entityName);
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
        if(isSpecialElemType(pool.elemType)) {
            LogWarning("[GameBoardElemPool::initPools] Can't have special elements of type '%d' in pools setup",
                pool.elemType);
            continue;
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

const char* GameBoardElemsPool::getElemEntityName(EBoardElemType elemType) const {
    const char* res = nullptr;
    switch(elemType) {
        case EBoardElemType::Red: {
            res = redEntity.c_str();
            break;
        }
        case EBoardElemType::Blue: {
            res = blueEntity.c_str();
            break;
        }
        case EBoardElemType::Green: {
            res = greenEntity.c_str();
            break;
        }
        case EBoardElemType::Purple: {
            res = purpleEntity.c_str();
            break;
        }
        case EBoardElemType::Yellow: {
            res = yellowEntity.c_str();
            break;
        }
        case EBoardElemType::HRocket: {
            res = hRocketEntity.c_str();
            break;
        }
        case EBoardElemType::VRocket: {
            res = vRocketEntity.c_str();
            break;
        }
        case EBoardElemType::Bomb: {
            res = bombEntity.c_str();
            break;
        }
        case EBoardElemType::Star: {
            res = starEntity.c_str();
            break;
        }
        case EBoardElemType::None: {
            [[fallthrough]];
        }
        default: {
            assert(false && "Invalid elem type");
        }
    }

    return res;
}

void GameBoardElemsPool::ET_setElemEntity(EBoardElemType elemType, const char* entityName) {
    switch(elemType) {
        case EBoardElemType::Red: {
            redEntity = entityName;
            break;
        }
        case EBoardElemType::Blue: {
            blueEntity = entityName;
            break;
        }
        case EBoardElemType::Green: {
            greenEntity = entityName;
            break;
        }
        case EBoardElemType::Purple: {
            purpleEntity = entityName;
            break;
        }
        case EBoardElemType::Yellow: {
            yellowEntity = entityName;
            break;
        }
        case EBoardElemType::HRocket: {
            hRocketEntity = entityName;
            break;
        }
        case EBoardElemType::VRocket: {
            vRocketEntity = entityName;
            break;
        }
        case EBoardElemType::Bomb: {
            bombEntity = entityName;
            break;
        }
        case EBoardElemType::Star: {
            starEntity = entityName;
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