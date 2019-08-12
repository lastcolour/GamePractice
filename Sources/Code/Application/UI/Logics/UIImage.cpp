#include "UI/Logics/UIImage.hpp"
#include "Core/JSONNode.hpp"
#include "ETApplicationInterfaces.hpp"

namespace {

const char* DEFAULT_IMAGER_RENDERER = "Render/RenderImageSimple.json";

} // namespace

UIImage::UIImage() {
}

UIImage::~UIImage() {
}

bool UIImage::serialize(const JSONNode& node) {
    UIBaseBox::serialize(node);
    std::string imageVal;
    node.value("image", imageVal);
    if(imageVal.empty()) {
        LogWarning("[UIImage::serialize] Empty image");
    }

    ET_SendEventReturn(imageRendererId, &ETGameObjectManager::ET_createGameObject, DEFAULT_IMAGER_RENDERER);
    return true;
}

bool UIImage::init() {
    UIBaseBox::init();
    return true;
}