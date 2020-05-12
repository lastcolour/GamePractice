#include "Laucher/GameLaucherModule.hpp"
#include "Laucher/GameLaucherConfig.hpp"
#include "Laucher/GameConfig.hpp"
#include "Laucher/GameLaucher.hpp"
#include "Reflect/ReflectContext.hpp"

GameLaucherModule::LogicsContainerPtrT GameLaucherModule::createSystemLogics() const {
    LogicsContainerPtrT container(
        new SystemLogicContainer<
            GameConfig,
            GameLaucher>()
        );
    return container;
}

void GameLaucherModule::reflectSystemConfigs(ReflectContext& ctx) const {
    if(auto classInfo = ctx.classInfo<GameLaucherModule>("GameLaucherModule")) {
        classInfo->addBaseClass<GameLaucherConfig>();
    }
}

void GameLaucherModule::registerEntityLogics(EntityLogicRegister& logicRegister) const {
}