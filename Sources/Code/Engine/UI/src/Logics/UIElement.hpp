#ifndef __UI_ELEMENT_HPP__
#define __UI_ELEMENT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntity.hpp"

class UIElement : public EntityLogic,
    public ETNode<ETUIElement>,
    public ETNode<ETEntityEvents> {
public:

    UIElement();
    virtual ~UIElement();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIElement
    void ET_setLayout(EntityId newLayoutId) override;
    EntityId ET_getLayout() override;
    void ET_setZIndex(int newZIndex) override;
    int ET_getZIndex() const override;
    int ET_getZIndexDepth() const override;
    void ET_show() override;
    void ET_hide() override;
    bool ET_isHidden() const override;
    float ET_getAlpha() const override;
    void ET_setAlpha(float newAlpha) override;
    void ET_enable() override;
    void ET_disable() override;
    bool ET_isEnabled() const override;
    void ET_setIgnoreTransform(bool flag) override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onAllLogicsCreated() override {}

protected:

    void updateLayout();

protected:

    virtual void onZIndexChanged(int newZIndex) = 0;
    virtual void onHide(bool flag) = 0;
    virtual void onAlphaChanged(float newAlpha) = 0;
    virtual void onDisabled(bool flag) = 0;
    virtual void onTransformChanged(const Transform& newTm) = 0;

protected:

    EntityId layoutId;
    float alpha;
    int zIndex;
    bool isIgnoringTransform;
    bool isHidden;
    bool isEnabled;
};

#endif /* __UI_ELEMENT_HPP__ */