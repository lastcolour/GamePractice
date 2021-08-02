#ifndef __UI_PROXY_CONTAINER_HPP__
#define __UI_PROXY_CONTAINER_HPP__

#include "UI/ETUIBox.hpp"

class UIProxyContainer : public ETNode<ETUIElementEvents> {
public:

    UIProxyContainer();
    ~UIProxyContainer();

    void setUIParent(EntityId elemId);
    EntityId getUIParent() const;
    void addItem(EntityId itemId, int extraZOffset);
    void removeItem(EntityId itemId);
    size_t count() const;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2D& newAabb) override {}
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override {}
    void ET_onIngoreTransform(bool flag) override {}

private:

    UIProxyContainer(const UIProxyContainer&) = delete;
    UIProxyContainer& operator=(const UIProxyContainer&) = delete;

private:

    struct ProxyData {
        int extraZOffset;
        bool prevVisble;
    };

private:

    EntityId parentId;
    std::unordered_map<EntityId, ProxyData> proxies;
};

#endif /* __UI_PROXY_CONTAINER_HPP__ */