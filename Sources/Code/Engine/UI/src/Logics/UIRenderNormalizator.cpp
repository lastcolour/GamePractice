#include "Logics/UIRenderNormalizator.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Render/ETRenderNode.hpp"
#include "UIUtils.hpp"

#include <algorithm>

void UIRenderNormalizator::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<UIRenderNormalizator>("UIRenderNormalizator")) {
        classInfo->addField("factor", &UIRenderNormalizator::factor);
    }
}

UIRenderNormalizator::UIRenderNormalizator() :
    factor(1.f) {
}

UIRenderNormalizator::~UIRenderNormalizator() {
}

void UIRenderNormalizator::init() {
    factor = std::max(0.001f, factor);
    ETNode<ETUIViewPortEvents>::connect(getEntityId());
    ETNode<ETEntityEvents>::connect(getEntityId());
}

void UIRenderNormalizator::deinit() {
}

void UIRenderNormalizator::ET_onTransformChanged(const Transform& newTm) {
}

void UIRenderNormalizator::ET_onLoaded() {
    float normFactor = UI::GetValueOnGrind(factor);
    ET_SendEvent(getEntityId(), &ETRenderNode::ET_setNormalizationScale, normFactor);
}

void UIRenderNormalizator::ET_onViewPortChanged(const Vec2i& newSize) {
    ET_onLoaded();
}