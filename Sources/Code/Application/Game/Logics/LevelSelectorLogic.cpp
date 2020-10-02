#include "Game/Logics/LevelSelectorLogic.hpp"
#include "Reflect/ReflectContext.hpp"

void LevelSelectorLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<LevelSelectorLogic>("LevelSelectorLogic")) {
        classInfo->addResourceField("level", ResourceType::Entity, &LevelSelectorLogic::setLevelName);
    }
}

LevelSelectorLogic::LevelSelectorLogic() {
}

LevelSelectorLogic::~LevelSelectorLogic() {
}

void LevelSelectorLogic::setLevelName(const char* newLevelName) {
    levelName = newLevelName;
}

bool LevelSelectorLogic::init() {
    ETNode<ETLevelSelector>::connect(getEntityId());
    return true;
}

void LevelSelectorLogic::deinit() {
}

const char* LevelSelectorLogic::ET_getLevelName() const {
    return levelName.c_str();
}
