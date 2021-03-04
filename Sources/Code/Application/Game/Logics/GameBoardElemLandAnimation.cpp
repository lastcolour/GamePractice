#include "Game/Logics/GameBoardElemLandAnimation.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Entity/ETEntity.hpp"
#include "Game/ETGameBoard.hpp"
#include "Audio/ETSound.hpp"

#include <cassert>
#include <algorithm>

namespace {

const float SCALE_DOWN_FRAC = 0.33f;
const float SCALE_UP_FRAC = 0.33f;
const float SCALE_BACK_FRAC = 0.33f;

const float X_SCALE_FACTOR = 0.1f;
const float Y_SCALE_FACTOR = 0.1f;

} // namespace

void GameBoardElemLandAnimation::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameBoardElemLandAnimation>("GameBoardElemLandAnimation")) {
        classInfo->addField("totalDuration", &GameBoardElemLandAnimation::totalDuration);
        classInfo->addResourceField("landSound", ResourceType::SoundEvent, &GameBoardElemLandAnimation::setLandSound);
    }
}

GameBoardElemLandAnimation::GameBoardElemLandAnimation() :
    elemLandPt(0.f),
    totalDuration(0.3f),
    currDuration(0.f),
    state(State::Finished) {
}

GameBoardElemLandAnimation::~GameBoardElemLandAnimation() {
}

void GameBoardElemLandAnimation::init() {
    ETNode<ETGameBoardElemLandAnimation>::connect(getEntityId());
}

void GameBoardElemLandAnimation::deinit() {
}

void GameBoardElemLandAnimation::ET_playLand() {
    assert(state == State::Finished && "Invalid anim state");

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getLocalTransform);
    elemLandPt = Vec2(tm.pt.x, tm.pt.y);

    state = State::ScaleDown;
    currDuration = 0.f;
    landSound.emit();
    ETNode<ETGameTimerEvents>::connect(getEntityId());
}

void GameBoardElemLandAnimation::ET_onGameTick(float dt) {
    currDuration += dt;

    Transform tm;
    ET_SendEventReturn(tm, getEntityId(), &ETEntity::ET_getLocalTransform);

    switch(state) {
        case State::ScaleDown: {
            float stateDur = std::max(0.001f, totalDuration * SCALE_DOWN_FRAC);
            float prog = std::min(1.f, currDuration / stateDur);
            tm.scale.x = Math::Lerp(1.f, 1.f + X_SCALE_FACTOR, prog);
            tm.scale.y = Math::Lerp(1.f, 1.f - Y_SCALE_FACTOR, prog);
            if(currDuration >= stateDur) {
                state = State::ScaleUp;
                currDuration = 0.f;
            }
            break;
        }
        case State::ScaleUp: {
            float stateDur = std::max(0.001f, totalDuration * SCALE_DOWN_FRAC);
            float prog = std::min(1.f, currDuration / stateDur);
            tm.scale.x = Math::Lerp(1.f + X_SCALE_FACTOR, 1.f - X_SCALE_FACTOR, prog);
            tm.scale.y = Math::Lerp(1.f - Y_SCALE_FACTOR, 1.f + Y_SCALE_FACTOR, prog);
            if(currDuration >= stateDur) {
                state = State::ScaleBack;
                currDuration = 0.f;
            }
            break;
        }
        case State::ScaleBack: {
            float stateDur = std::max(0.001f, totalDuration * SCALE_DOWN_FRAC);
            float prog = std::min(1.f, currDuration / stateDur);
            tm.scale.x = Math::Lerp(1.f - X_SCALE_FACTOR, 1.f, prog);
            tm.scale.y = Math::Lerp(1.f + Y_SCALE_FACTOR, 1.f, prog);
            if(currDuration >= stateDur) {
                tm.scale = Vec3(1.f);
                state = State::Finished;
                ET_SendEvent(getEntityId(), &ETGameBoardElem::ET_onLandPlayed);
                ETNode<ETGameTimerEvents>::disconnect();
            }
            break;
        }
        case State::Finished: {
            break;
        }
        default: {
            assert(false && "Invalid anim state");
        }
    }

    tm.pt = Vec3(elemLandPt, 0.f);

    int cellSize = 0;
    ET_SendEventReturn(cellSize, &ETGameBoard::ET_getCellSize);

    float newYCellSize = tm.scale.y * cellSize;
    tm.pt.y -= (cellSize - newYCellSize) / 2.f;

    ET_SendEvent(getEntityId(), &ETEntity::ET_setLocalTransform, tm);
}

void GameBoardElemLandAnimation::setLandSound(const char* soundEventName) {
    ET_SendEventReturn(landSound, &ETSoundManager::ET_createEvent, soundEventName);
}