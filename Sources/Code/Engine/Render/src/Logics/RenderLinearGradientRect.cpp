#include "Logics/RenderLinearGradientRect.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderProxyNode.hpp"

RenderLinearGradientRect::RenderLinearGradientRect() :
    RenderNode(RenderNodeType::Gradient),
    size(100),
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

bool RenderLinearGradientRect::init() {
    RenderNode::init();
    ETNode<ETRenderRect>::connect(getEntityId());
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setColor0, startCol);
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setColor1, endCol);
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setVertical, isVertical);
    ET_setSize(size);
    if(ET_isVisible()) {
        ET_show();
    }
    return true;
}

void RenderLinearGradientRect::ET_setSize(const Vec2i& newSize) {
    size = newSize;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setSize, newSize);
}

Vec2i RenderLinearGradientRect::ET_getSize() const {
    return size;
}