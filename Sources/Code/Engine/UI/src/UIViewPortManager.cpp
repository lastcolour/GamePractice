#include "UIViewPortManager.hpp"

UIViewPortManager::UIViewPortManager() :
    viewPort(0) {
}

UIViewPortManager::~UIViewPortManager() {
}

bool UIViewPortManager::init() {
    ETNode<ETUIViewPort>::connect(getEntityId());
    ETNode<ETRenderCameraEvents>::connect(getEntityId());
    return true;
}

void UIViewPortManager::deinit() {
}

const Vec2i& UIViewPortManager::ET_getViewport() const {
    return viewPort;
}

void UIViewPortManager::ET_onRenderPortResized(const Vec2i& newSize) {
    viewPort = newSize;
    ET_SendEvent(&ETUIViewPortEvents::ET_onViewPortChanged, newSize);
}