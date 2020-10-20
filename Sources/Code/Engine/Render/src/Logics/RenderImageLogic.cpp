#include "Logics/RenderImageLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ETRenderProxyNode.hpp"

RenderImageLogic::RenderImageLogic() :
    RenderImageLogic(RenderNodeType::Image) {
}

RenderImageLogic::RenderImageLogic(RenderNodeType nodeType) :
    RenderNode(nodeType),
    size(100) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderImageLogic::size);
        classInfo->addResourceField("image", ResourceType::Image, &RenderImageLogic::ET_setImage);
    }
}

bool RenderImageLogic::init() {
    RenderNode::init();
    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    ET_setImage(image.c_str());
    ET_setSize(size);
    return true;
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    image = imageName;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setImage, image);
}

Vec2i RenderImageLogic::ET_getImageSize() const {
    return Vec2i(0);
}

void RenderImageLogic::ET_setSize(const Vec2i& newSize) {
    size = newSize;
    ET_QueueEvent(renderNodeId, &ETRenderProxyNode::ET_setSize, newSize);
}

Vec2i RenderImageLogic::ET_getSize() const {
    return size;
}