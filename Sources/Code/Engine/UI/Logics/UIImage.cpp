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
    node.read("image", image);
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
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETRenderImageLogic::ET_setImage, image.c_str());
        forceResizeFromTop();
    }
}

Vec2i UIImage::calculateBoxSize(const AABB2Di& parentBox) const {
    auto boxSize = UIBaseBox::calculateBoxSize(parentBox);

    Vec2i imageSize(0);
    ET_SendEventReturn(imageSize, renderId, &ETRenderImageLogic::ET_getOriginalSize);
    if(imageSize.x == 0 || imageSize.y == 0) {
        return Vec2i(0);
    }

    Vec2 scale = Vec2(boxSize.x / static_cast<float>(imageSize.x),
        boxSize.y / static_cast<float>(imageSize.y));

    ET_SendEvent(renderId, &ETRenderImageLogic::ET_setScale, scale);

    return boxSize;
}

void UIImage::createRenderer() {
    if(renderId.isValid()) {
        ET_SendEvent(&ETEntityManager::ET_destroyEntity, renderId);
        renderId = InvalidEntityId;
    }
    const auto& boxStyle = ET_getStyle();
    std::string rendererName = boxStyle.renderer;
    if(rendererName.empty()) {
        rendererName = DEFAULT_IMAGER_RENDERER;
    }
    ET_SendEventReturn(renderId, &ETEntityManager::ET_createEntity, rendererName.c_str());
    if(renderId.isValid()) {
        ET_SendEvent(renderId, &ETRenderImageLogic::ET_setImage, image.c_str());
        setUpRenderChild(renderId);
    } else {
        LogWarning("[UIImage::createRenderers] Can't create renderer: %s", rendererName);
    }
}

EntityId UIImage::getRenderId() const {
    return renderId;
}