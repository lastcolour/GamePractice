#include "Logics/RenderSimpleLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderProxyNode.hpp"

RenderSimpleLogic::RenderSimpleLogic() :
    RenderNode(RenderNodeType::Simple),
    size(20.f),
    color(255, 255, 255) {
}

RenderSimpleLogic::~RenderSimpleLogic() {
}

void RenderSimpleLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderSimpleLogic>("RenderSimple")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderSimpleLogic::size);
        classInfo->addField("color", &RenderSimpleLogic::color);
    }
}

bool RenderSimpleLogic::init() {
    RenderNode::init();
    ETNode<ETRenderSimpleLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    ET_setColor(color);
    ET_setSize(size);
    return true;
}

void RenderSimpleLogic::ET_setColor(const ColorB& col) {
    color = col;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setColor0, col);
}

void RenderSimpleLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setSize, newSize);
}

Vec2 RenderSimpleLogic::ET_getSize() const {
    return size;
}