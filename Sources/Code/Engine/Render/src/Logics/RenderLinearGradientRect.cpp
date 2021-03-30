#include "Logics/RenderLinearGradientRect.hpp"
#include "Nodes/GradientNode.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    RenderNode(RenderNodeType::Gradient),
    size(100.f),
    startCol(255, 255, 255),
    endCol(0, 0, 0),
    isVertical(true) {
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

void RenderLinearGradientRect::onInit() {
    auto gradientProxyNode = static_cast<GradientNode*>(proxyNode);
    gradientProxyNode->setColor0(startCol);
    gradientProxyNode->setColor1(endCol);
    gradientProxyNode->setVertical(isVertical);
    gradientProxyNode->setSize(size);

    ETNode<ETRenderRect>::connect(getEntityId());
}

void RenderLinearGradientRect::ET_setSize(const Vec2& newSize) {
    size = newSize;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newSize](){
        auto gradientProxyNode = static_cast<GradientNode*>(node);
        gradientProxyNode->setSize(newSize);
    });
}

Vec2 RenderLinearGradientRect::ET_getSize() const {
    return size;
}