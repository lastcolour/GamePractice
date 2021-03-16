#include "Logics/RenderLinearGradientRect.hpp"
#include "Nodes/GradientNode.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    RenderNode(RenderNodeType::Gradient),
    size(100.f),
    startCol(255, 255, 255),
    endCol(0, 0, 0),
    isVertical(true),
    isSizeChanged(true),
    isColorChanged(true) {
}

RenderLinearGradientRect::~RenderLinearGradientRect() {
}

void RenderLinearGradientRect::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderLinearGradientRect>("RenderLinearGradientRect")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderLinearGradientRect::size);
        classInfo->addField("startColor", &RenderLinearGradientRect::startCol);
        classInfo->addField("endColor", &RenderLinearGradientRect::endCol);
        classInfo->addField("isVertical", &RenderLinearGradientRect::isVertical);
    }
}

void RenderLinearGradientRect::init() {
    RenderNode::init();

    if(!proxyNode) {
        return;
    }

    ETNode<ETRenderRect>::connect(getEntityId());
    isColorChanged = true;
    ET_setSize(size);
}

void RenderLinearGradientRect::ET_setSize(const Vec2& newSize) {
    size = newSize;
    isSizeChanged = true;
    markForSyncWithRender();
}

Vec2 RenderLinearGradientRect::ET_getSize() const {
    return size;
}

void RenderLinearGradientRect::onSyncWithRender() {
    auto gradientProxyNode = static_cast<GradientNode*>(proxyNode);
    if(isSizeChanged) {
        isSizeChanged = false;
        gradientProxyNode->setSize(size);
    }
    if(isColorChanged) {
        isColorChanged = false;
        gradientProxyNode->setColor0(startCol);
        gradientProxyNode->setColor1(endCol);
        gradientProxyNode->setVertical(isVertical);
    }
}