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

void UIViewPortManager::ET_update() {
    auto oldViewPort = viewPort;
    ET_PollAllEvents<ETRenderCameraEvents>();
    if(oldViewPort != viewPort) {
        LogDebug("[UIViewPortManager::ET_update] Set viewport: [%dx%d]", viewPort.x, viewPort.y);
        ET_SendEvent(&ETUIViewPortEvents::ET_onViewPortChanged, viewPort);
    }
}

void UIViewPortManager::ET_onRenderPortResized(const Vec2i& newSize) {
    viewPort = newSize;
}

void UIViewPortManager::ET_setViewPort(const Vec2i& newSize) {
    if(viewPort == newSize) {
        return;
    }
    viewPort = newSize;
    LogDebug("[UIViewPortManager::ET_setViewPort] Set viewport: [%dx%d]", viewPort.x, viewPort.y);
    ET_SendEvent(&ETUIViewPortEvents::ET_onViewPortChanged, newSize);
}