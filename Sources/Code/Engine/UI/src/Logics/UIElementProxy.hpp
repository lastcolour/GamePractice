#ifndef __UI_ELEMENT_PROXY_HPP__
#define __UI_ELEMENT_PROXY_HPP__

#include "Logics/UIElement.hpp"
#include "UI/ETUIViewPort.hpp"

class UIProxyNode {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIProxyNode();

public:

    EntityId entId;
    float normScale;
    int extraZOffset;
};

class UIElementProxy : public UIElement,
    public ETNode<ETUIViewPortEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIElementProxy();
    virtual ~UIElementProxy();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIElement
    AABB2D ET_getBox() const override;
    UIBoxMargin ET_getMargin() const override;

    // ETEntityEvents
    void ET_onLoaded() override;

    // ETUIViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;

protected:

    void onZIndexChanged(int newZIndex) override;
    void onHide(bool flag) override;
    void onAlphaChanged(float newAlpha) override;
    void onDisabled(bool flag) override {}
    void onTransformChanged(const Transform& newTm) override {}

private:

    void applyNormScale();

private:

    std::vector<UIProxyNode> nodes;
};

#endif /* __UI_ELEMENT_PROXY_HPP__ */