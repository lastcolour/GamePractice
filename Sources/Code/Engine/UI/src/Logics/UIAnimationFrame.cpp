#include "Logics/UIAnimationFrame.hpp"
#include "Reflect/EnumInfo.hpp"

void UIAnimationFrame::Reflect(ReflectContext& ctx) {
    if(auto enumInfo = ctx.enumInfo<EAnimLerpMode>("EAnimLerpMode")) {
        enumInfo->addValues<EAnimLerpMode>({
            {"Linear", EAnimLerpMode::Linear},
            {"Pow_2", EAnimLerpMode::Pow_2},
            {"Pow_4", EAnimLerpMode::Pow_4},
            {"InvPow_2", EAnimLerpMode::InvPow_2},
            {"InvPow_4", EAnimLerpMode::InvPow_4},
            {"Sin", EAnimLerpMode::Sin},
            {"Sin_Sqr_2", EAnimLerpMode::Sin_Sqr_2},
            {"InvSin_Sqr_2", EAnimLerpMode::InvSin_Sqr_2}
        });
    }
    if(auto classInfo = ctx.classInfo<UIAnimationFrame>("UIAnimationFrame")) {
        classInfo->addField("lerpMode", &UIAnimationFrame::lerpMode);
        classInfo->addField("duration", &UIAnimationFrame::duration);
        classInfo->addField("offset", &UIAnimationFrame::offset);
        classInfo->addField("scale", &UIAnimationFrame::scale);
        classInfo->addField("alpha", &UIAnimationFrame::alpha);
    }
}

UIAnimationFrame::UIAnimationFrame() :
    offset(0),
    scale(1.f),
    duration(1.f),
    alpha(1.f),
    lerpMode(EAnimLerpMode::Linear),
    state(EAnimFrameState::Pending) {
}