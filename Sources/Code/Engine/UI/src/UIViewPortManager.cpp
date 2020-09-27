#include "UIViewPortManager.hpp"

UIViewPortManager::UIViewPortManager() :
    viewPort(0),
    viewPortUpdated(false) {
}

UIViewPortManager::~UIViewPortManager() {
}

bool UIViewPortManager::init() {
   ET_SendEventReturn(viewPort, &ETRenderCamera::ET_getRenderPort);

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
    ET_setViewPort(newSize);
}

void UIViewPortManager::ET_setViewPort(const Vec2i& newSize) {
    if(viewPort != newSize) {
        viewPort = newSize;
        ET_SendEvent(&ETUIViewPortEvents::ET_onViewPortChanged, newSize);
    }
}