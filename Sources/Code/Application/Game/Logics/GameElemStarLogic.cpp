#include "Game/Logics/GameElemStarLogic.hpp"
#include "Game/ETGameBoard.hpp"
#include "Game/Logics/GameBoardUtils.hpp"

namespace {

struct EntityIdAngle {
    EntityId entId;
    float angle;
};

void sortEntitiesByDirection(std::vector<EntityId>& elems, const Vec2& center) {
    std::vector<EntityIdAngle> entitiesAndAngles;
    entitiesAndAngles.reserve(elems.size());

    for(auto& entId : elems) {
        Transform tm;
        ET_SendEventReturn(tm, entId, &ETEntity::ET_getTransform);

        Vec2 pt(tm.pt.x, tm.pt.y);
        auto dir = pt - center;
        dir.normalize();

        EntityIdAngle entIdAngle;
        entIdAngle.entId = entId;
        entIdAngle.angle = std::atan2(dir.y, dir.x);
        entitiesAndAngles.push_back(entIdAngle);
    }

    std::sort(entitiesAndAngles.begin(), entitiesAndAngles.end(), [](const EntityIdAngle& first, const EntityIdAngle& second){
        return first.angle > second.angle;
    });

    for(size_t i = 0, sz = entitiesAndAngles.size(); i < sz; ++i) {
        elems[i] = entitiesAndAngles[i].entId;
    }
}

} // namespace

void GameElemStarLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemStarLogic>("GameElemStarLogic")) {
        classInfo->addField("destroyDelay", &GameElemStarLogic::destroyDelay);
    }
}

GameElemStarLogic::GameElemStarLogic() :
    startPt(-1),
    destroyDelay(0.05f),
    elemType(EBoardElemType::None) {
}

GameElemStarLogic::~GameElemStarLogic() {
}

void GameElemStarLogic::init() {
    ETNode<ETGameBoardElemTriggerLogic>::connect(getEntityId());
}

void GameElemStarLogic::deinit() {
}

void GameElemStarLogic::ET_setSwapedElem(EntityId elemId) {
    elemType = GameUtils::GetElemType(elemId);
}

void GameElemStarLogic::ET_start() {
    ET_SendEventReturn(startPt, &ETGameBoard::ET_getElemBoardPos, getEntityId());


    std::vector<EntityId> destroyElems;
    if(elemType == EBoardElemType::None) {
        for(auto otherElemType : {EBoardElemType::Blue, EBoardElemType::Red, EBoardElemType::Yellow,
            EBoardElemType::Green, EBoardElemType::Purple}) {
            std::vector<EntityId> otherElems;
            ET_SendEventReturn(otherElems, &ETGameBoard::ET_getAllElemsOfType, otherElemType);
            if(otherElems.size() > destroyElems.size()) {
                destroyElems = std::move(otherElems);
            }
        }
    } else {
        ET_SendEventReturn(destroyElems, &ETGameBoard::ET_getAllElemsOfType, elemType);
    }

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getTransform);

    sortEntitiesByDirection(destroyElems, Vec2(tm.pt.x, tm.pt.y));

    float currDelay = destroyDelay;
    for(auto& entId : destroyElems) {
        DestroyTask task;
        task.entId = entId;
        task.delay = currDelay;
        destroyTasks.push_back(task);

        currDelay += destroyDelay;
    }
}

bool GameElemStarLogic::ET_update(float dt) {
    auto it = destroyTasks.begin();
    while(it != destroyTasks.end()) {
        it->delay -= dt;
        if(it->delay < 0.f) {
            GameUtils::TryTriggerElemDestroy(getEntityId(), it->entId);
            it = destroyTasks.erase(it);
        } else {
            ++it;
        }
    }
    if(!destroyTasks.empty()) {
        return false;
    }

    elemType = EBoardElemType::None;

    ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onTriggerDone);
    return true;
}