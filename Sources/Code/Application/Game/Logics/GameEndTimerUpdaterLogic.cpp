#include "Game/Logics/GameEndTimerUpdaterLogic.hpp"
#include "UI/UIETInterfaces.hpp"
#include "Core/StringFormat.hpp"

GameEndTimerUpdaterLogic::GameEndTimerUpdaterLogic() {
}

GameEndTimerUpdaterLogic::~GameEndTimerUpdaterLogic() {
}

bool GameEndTimerUpdaterLogic::serialize(const JSONNode& node) {
    (void)node;
    return true;
}

bool GameEndTimerUpdaterLogic::init() {
    ETNode<ETGameEndTimerUpdater>::connect(getEntityId());
    return true;
}

void GameEndTimerUpdaterLogic::ET_setEndTime(float endTime) {
    std::string text = StringFormat("%.1f", endTime);
    ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, text.c_str());
}