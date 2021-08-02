#include "UI/UIProxyContainer.hpp"
#include "Render/ETRenderNode.hpp"

UIProxyContainer::UIProxyContainer() {
}

UIProxyContainer::~UIProxyContainer() {
}

EntityId UIProxyContainer::getUIParent() const {
    return parentId;
}

void UIProxyContainer::setUIParent(EntityId newParentId) {
    if(parentId == newParentId) {
        return;
    } else {
        ETNode<ETUIElementEvents>::disconnect();
    }

    bool prevHidden = true;
    ET_SendEventReturn(prevHidden, parentId, &ETUIElement::ET_isHidden);
    bool currHidden = true;
    ET_SendEventReturn(currHidden, newParentId, &ETUIElement::ET_isHidden);

    parentId = newParentId;
    if(!parentId.isValid()) {
        return;
    } else {
         ETNode<ETUIElementEvents>::connect(newParentId);
    }

    int zIndex = 0;
    ET_SendEventReturn(zIndex, newParentId, &ETUIElement::ET_getZIndex);
    ET_onZIndexChanged(zIndex);

    if(prevHidden != currHidden) {
        ET_onHidden(currHidden);
    }

    float alpha = 1.f;
    ET_SendEventReturn(alpha, newParentId, &ETUIElement::ET_getAlpha);
    ET_onAlphaChanged(alpha);
}

void UIProxyContainer::addItem(EntityId itemId, int extraZOffset) {
    if(!itemId.isValid()) {
        return;
    }

    ProxyData proxyData = {extraZOffset, false};
    ET_SendEventReturn(proxyData.prevVisble, itemId, &ETRenderNode::ET_isVisible);

    auto it = proxies.find(itemId);
    if(it == proxies.end()) {
        proxies[itemId] = proxyData;
    } else {
        it->second = proxyData;
    }

    int zIndex = 0;
    ET_SendEventReturn(zIndex, parentId, &ETUIElement::ET_getZIndex);
    ET_SendEvent(itemId, &ETRenderNode::ET_setDrawPriority, zIndex + extraZOffset);

    bool isHidden = true;
    ET_SendEventReturn(isHidden, parentId, &ETUIElement::ET_isHidden);
    if(isHidden) {
        ET_SendEvent(itemId, &ETRenderNode::ET_hide);
    }

    float alpha = 1.f;
    ET_SendEventReturn(alpha, parentId, &ETUIElement::ET_getAlpha);
    ET_SendEvent(itemId, &ETRenderNode::ET_setAlphaMultiplier, alpha);
}

void UIProxyContainer::removeItem(EntityId itemId) {
    if(!itemId.isValid()) {
        return;
    }
    auto it =  proxies.find(itemId);
    if(it != proxies.end()) {
        proxies.erase(it);
    }
}

size_t UIProxyContainer::count() const {
    return proxies.size();
}

void UIProxyContainer::ET_onZIndexChanged(int newZIndex) {
    for(auto& node : proxies) {
        ET_SendEvent(node.first, &ETRenderNode::ET_setDrawPriority, newZIndex + node.second.extraZOffset);
    }
}

void UIProxyContainer::ET_onAlphaChanged(float newAlpha) {
    for(auto& node : proxies) {
        ET_SendEvent(node.first, &ETRenderNode::ET_setAlphaMultiplier, newAlpha);
    }
}

void UIProxyContainer::ET_onHidden(bool flag) {
    if(flag) {
        for(auto& node : proxies) {
            ET_SendEventReturn(node.second.prevVisble, node.first, &ETRenderNode::ET_isVisible);
            ET_SendEvent(node.first, &ETRenderNode::ET_hide);
        }
    } else {
        for(auto& node : proxies) {
            if(node.second.prevVisble) {
                ET_SendEvent(node.first, &ETRenderNode::ET_show);
            }
        }
    }
}