#ifndef __UI_ELEMENT_PROXY_HPP__
#define __UI_ELEMENT_PROXY_HPP__

#include "Logics/UIElement.hpp"
#include "UI/ETUIViewPort.hpp"

class UIElementProxy : public UIElement,
    public ETNode<ETUIViewPortEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIElementProxy();
    virtual ~UIElementProxy();

    // ETUIElement
    void ET_setRenderId(EntityId newRenderId) override {}
    EntityId ET_getRenderId(EntityId newRenderId) const override { return InvalidEntityId; }

    // EntityLogic
    void init() override;
    void deinit() override;
    void onLoaded() override;

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

    float gridScale;
    bool useGridScale;
};

#endif /* __UI_ELEMENT_PROXY_HPP__ */