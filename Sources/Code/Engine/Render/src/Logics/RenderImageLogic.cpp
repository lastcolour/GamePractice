#include "Logics/RenderImageLogic.hpp"
#include "Nodes/ImageNode.hpp"

RenderImageLogic::RenderImageLogic() :
    RenderImageLogic(RenderNodeType::Image) {
}

RenderImageLogic::RenderImageLogic(RenderNodeType nodeType) :
    RenderNode(nodeType),
    size(100.f),
    tintColor(255, 255, 255, 0) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderImageLogic::size);
        classInfo->addField("tintColor", &RenderImageLogic::tintColor);
        classInfo->addResourceField("image", ResourceType::Image, &RenderImageLogic::image);
    }
}

void RenderImageLogic::onInit() {
    auto imageProxyNode = static_cast<ImageNode*>(proxyNode);
    imageProxyNode->setTintColor(tintColor);
    imageProxyNode->setSize(size);

    ET_setImage(image.c_str());

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    image = imageName;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, img=image](){
        auto imageProxyNode = static_cast<ImageNode*>(node);
        imageProxyNode->setImage(img);
    });
}

void RenderImageLogic::ET_setTintColor(const ColorB& newTintColor) {
    tintColor = newTintColor;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newTintColor](){
        auto imageProxyNode = static_cast<ImageNode*>(node);
        imageProxyNode->setTintColor(newTintColor);
    });
}

void RenderImageLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    ET_QueueEvent(&ETRenderNodeManager::ET_addUpdateEvent, [node=proxyNode, newSize](){
        auto imageProxyNode = static_cast<ImageNode*>(node);
        imageProxyNode->setSize(newSize);
    });
}

Vec2 RenderImageLogic::ET_getSize() const {
    return size;
}