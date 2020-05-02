#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Core/StringFormat.hpp"

GameEndTimerUpdaterLogic::GameEndTimerUpdaterLogic() {
}

GameEndTimerUpdaterLogic::~GameEndTimerUpdaterLogic() {
}

void GameEndTimerUpdaterLogic::Reflect(ReflectContext& ctx) {
    ctx.classInfo<GameEndTimerUpdaterLogic>("GameEndTimerUpdater");
}

bool GameEndTimerUpdaterLogic::init() {
    ETNode<ETGameEndTimerUpdater>::connect(getEntityId());
    return true;
}

void GameEndTimerUpdaterLogic::deinit() {
}

void GameEndTimerUpdaterLogic::ET_setEndTime(float endTime) {
    std::string text = StringFormat("%.1f", endTime);
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}