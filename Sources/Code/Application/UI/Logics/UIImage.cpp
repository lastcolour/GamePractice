#include "UI/Logics/UIImage.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

#include <algorithm>

namespace {

const char* DEFAULT_IMAGER_RENDERER = "Render/RenderImageSimple.json";

} // namespace

UIImage::UIImage() {
}

UIImage::~UIImage() {
}

bool UIImage::serialize(const JSONNode& node) {
    UIBaseBox::serialize(node);
    node.value("image", image);
    if(image.empty()) {
        LogWarning("[UIImage::serialize] Empty image");
    }
    return true;
}

bool UIImage::init() {
    createRenderer();
    UIBaseBox::init();
    ETNode<ETUIImage>::connect(getEntityId());
    return true;
}

void UIImage::ET_setImage(const char* newImage) {
    image = newImage;
    if(imageRendererId.isValid()) {
        ET_SendEvent(imageRendererId, &ETRenderImageLogic::ET_setImage, image.c_str());
        forceResizeFromTop();
    }
}

Vec2i UIImage::calculateBoxSize(const AABB2Di& parentBox) const {
    auto parentSize = parentBox.getSize();
    Vec2i imageSize(0);
    ET_SendEventReturn(imageSize, imageRendererId, &ETRenderImageLogic::ET_getOrigSize);
    if(imageSize.x == 0 || imageSize.y == 0) {
        return Vec2i(0);
    }

    float scale = std::min(parentSize.x / static_cast<float>(imageSize.x),
        parentSize.y / static_cast<float>(imageSize.y));

    ET_SendEvent(imageRendererId, &ETRenderImageLogic::ET_setScale, Vec2(scale));

    imageSize = Vec2i(0);
    ET_SendEventReturn(imageSize, imageRendererId, &ETRenderImageLogic::ET_getSize);

    return imageSize;
}

void UIImage::createRenderer() {
    if(imageRendererId.isValid()) {
        ET_SendEvent(&ETGameObjectManager::ET_destroyObject, imageRendererId);
        imageRendererId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty()) {
        rendererName = DEFAULT_IMAGER_RENDERER;
    }
    ET_SendEventReturn(imageRendererId, &ETGameObjectManager::ET_createGameObject, rendererName.c_str());
    if(imageRendererId.isValid()) {
        ET_SendEvent(imageRendererId, &ETGameObject::ET_setParent, getEntityId());
        ET_SendEvent(imageRendererId, &ETRenderImageLogic::ET_setImage, image.c_str());

        Transform tm;
        ET_SendEventReturn(tm, getEntityId(), &ETGameObject::ET_getTransform);
        ET_SendEvent(imageRendererId, &ETGameObject::ET_setTransform, tm);
    } else {
        LogWarning("[UIImage::createRenderers] Can't create renderer: %s", rendererName);
    }
}
