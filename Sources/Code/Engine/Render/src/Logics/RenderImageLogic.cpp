#include "Logics/RenderImageLogic.hpp"
#include "Reflect/ReflectContext.hpp"
#include "Nodes/ImageNode.hpp"

RenderImageLogic::RenderImageLogic() :
    RenderImageLogic(RenderNodeType::Image) {
}

RenderImageLogic::RenderImageLogic(RenderNodeType nodeType) :
    RenderNode(nodeType),
    size(100.f),
    tintColor(255, 255, 255, 0),
    isImageChanged(true),
    isSizeChanged(true) {
}

RenderImageLogic::~RenderImageLogic() {
}

void RenderImageLogic::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<RenderImageLogic>("RenderImage")) {
        classInfo->addBaseClass<RenderNode>();
        classInfo->addField("size", &RenderImageLogic::size);
        classInfo->addField("tintColor", &RenderImageLogic::tintColor);
        classInfo->addResourceField("image", ResourceType::Image, &RenderImageLogic::ET_setImage);
    }
}

void RenderImageLogic::init() {
    RenderNode::init();

    if(!proxyNode) {
        return;
    }

    ETNode<ETRenderImageLogic>::connect(getEntityId());
    ETNode<ETRenderRect>::connect(getEntityId());
    ET_setImage(image.c_str());
    ET_setSize(size);
    ET_setTintColor(tintColor);
}

void RenderImageLogic::ET_setImage(const char* imageName) {
    image = imageName;
    isImageChanged = true;
    markForSyncWithRender();
}

void RenderImageLogic::ET_setTintColor(const ColorB& newTintColor) {
    tintColor = newTintColor;
    isTintColorChanged = true;
    markForSyncWithRender();
}

void RenderImageLogic::ET_setSize(const Vec2& newSize) {
    size = newSize;
    isSizeChanged = true;
    markForSyncWithRender();
}

Vec2 RenderImageLogic::ET_getSize() const {
    return size;
}

void RenderImageLogic::onSyncWithRender() {
    auto imageProxyNode = static_cast<ImageNode*>(proxyNode);
    if(isSizeChanged) {
        isSizeChanged = false;
        imageProxyNode->setSize(size);
    }
    if(isImageChanged) {
        isImageChanged = false;
        imageProxyNode->setImage(image);
    }
    if(isTintColorChanged) {
        isTintColorChanged = false;
        imageProxyNode->setTintColor(tintColor);
    }
}