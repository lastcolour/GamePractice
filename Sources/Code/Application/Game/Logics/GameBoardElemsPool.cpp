#include "Game/Logics/GameBoardElemsPool.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntityManger.hpp"
#include "Entity/ETEntity.hpp"
#include "Core/ETLogger.hpp"

#include <cassert>

void ElementDescriptor::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<ElementDescriptor>("ElementDescriptor")) {
        classInfo->addResourceField("object", ResourceType::Entity, &ElementDescriptor::object);
        classInfo->addField("weight", &ElementDescriptor::weight);
    }
}

ElementDescriptor::ElementDescriptor() :
    weight(1) {
}

void GameBoardElemsPool::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardElemsPool>("GameBoardElemsPool")) {
        classInfo->addField("elems", &GameBoardElemsPool::poolsInfo);
    }
}

GameBoardElemsPool::GameBoardElemsPool() {
}

GameBoardElemsPool::~GameBoardElemsPool() {
}

void GameBoardElemsPool::init() {
    initGenerator();
    ETNode<ETGameBoardElemsPool>::connect(getEntityId());
}

void GameBoardElemsPool::deinit() {
    for(auto& elemNode : elements) {
        auto elemId = elemNode.first;
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, elemId);
    }
    elements.clear();
}

EntityId GameBoardElemsPool::ET_spawnElem() {
    int currWeight = intGenerator.generate();
    int spawnPoolId = -1;
    for(int i = 0, sz = static_cast<int>(poolsInfo.size()); i < sz; ++i) {
        auto& poolInfo = poolsInfo[i];
        if((poolInfo.weight - currWeight) > 0) {
            spawnPoolId = i;
            break;
        } else {
            currWeight -= poolInfo.weight;
        }
    }

    assert(spawnPoolId != -1 && "Invalid spawn pool id");

    EntityId elemId;
    for(auto& elemNode : elements) {
        auto& elemInfo = elemNode.second;
        if(elemInfo.poolId == spawnPoolId && !elemInfo.isAlive) {
            elemInfo.isAlive = true;
            elemId = elemNode.first;
            break;
        }
    }

    if(elemId.isValid()) {
        return elemId;
    }

    auto& poolInfo = poolsInfo[spawnPoolId];
    ET_SendEventReturn(elemId, &ETEntityManager::ET_createEntity, poolInfo.object.c_str());
    if(!elemId.isValid()) {
        LogWarning("[GameBoardElemsPool::ET_spawnElem] Can't spawn a new element: '%s'",
            poolInfo.object);
        return elemId;
    }

    ET_SendEvent(getEntityId(), &ETEntity::ET_addChild, elemId);

    elements[elemId] = ElemInfo{spawnPoolId, true};

    return elemId;
}

void GameBoardElemsPool::ET_removeElem(EntityId elemId) {
    auto it = elements.find(elemId);
    if(it == elements.end()) {
        LogError("[GameBoardElemsPool::ET_removeElem] Can't find spawned elem: '%s'",
            EntityUtils::GetEntityName(elemId));
        return;
    }
    auto& elemInfo = it->second;
    elemInfo.isAlive = false;
}

void GameBoardElemsPool::initGenerator() {
    int totalWeight = 0;
    for(auto& elem : poolsInfo) {
        if(elem.weight < 0) {
            LogWarning("[GameBoardElemPool::init] Elements of type: '%s' have negative weight: '%d'",
                elem.object, elem.weight);
            elem.weight = 0;
        }
        totalWeight += elem.weight;
    }
    if(totalWeight > 0) {
        intGenerator.setRange(0, totalWeight - 1);
    }
}

void GameBoardElemsPool::ET_addElemsToSpawn(const ElementDescriptor& newElemDescr) {
    poolsInfo.push_back(newElemDescr);
    initGenerator();
}
