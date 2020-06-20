#include "UI/Logics/UILabelSetter.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

UILabelSetter::UILabelSetter() :
    valueType(EValueType::None) {
}

UILabelSetter::~UILabelSetter() {
}

void UILabelSetter::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EValueType>("UILabelSetter")) {
        enumInfo->addValues<EValueType>({
            {"Score", EValueType::GameScore},
            {"HightSocre", EValueType::GameHighScore},
        });
    };
    if(auto classInfo = ctx.classInfo<UILabelSetter>("UILabelSetter")) {
        classInfo->addField("value", &UILabelSetter::valueType);
    }
}

bool UILabelSetter::init() {
    switch(valueType)
    {
    case EValueType::GameScore: {
        const EndGameResult* endResult = nullptr;
        ET_SendEventReturn(endResult, &ETGameEndResult::ET_getGameEndResult);
        if(endResult) {
            std::string labelText = StringFormat("%d", endResult->score);
            ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, labelText.c_str());
        } else {
            LogWarning("[UILabelSetter::init] Can't query game end result");
        }
        break;
    }
    case EValueType::GameHighScore: {
        int score = 0;
        ET_SendEventReturn(score, &ETGameConfig::ET_getHighScore);
        std::string labelText = StringFormat("%d", score);
        ET_SendEvent(getEntityId(), &ETUILabel::ET_setText, labelText.c_str());
        break;
    }
    case EValueType::None:
    default:
        return false;
    }
    return true;
}

void UILabelSetter::deinit() {
}