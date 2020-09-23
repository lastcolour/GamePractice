#include "UIViewConfig.hpp"
#include "Reflect/ReflectContext.hpp"

UIViewConfig::UIViewConfig() {
}

UIViewConfig::~UIViewConfig() {
}

void UIViewConfig::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIViewConfig>("UIViewConfig")) {
        classInfo->addField("mainView", &UIViewConfig::mainView);
        classInfo->addField("gameView", &UIViewConfig::gameView);
        classInfo->addField("backgroundView", &UIViewConfig::backgroundView);
        classInfo->addField("endGameView", &UIViewConfig::endGameView);
        classInfo->addField("loadingView", &UIViewConfig::loadingView);
        classInfo->addField("pauseView", &UIViewConfig::pauseView);
    }
}