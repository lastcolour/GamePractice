#ifndef __UI_ELEMENT_HPP__
#define __UI_ELEMENT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntity.hpp"

class ReflectContext;

class UIElement : public EntityLogic,
    public ETNode<ETUIElement>,
    public ETNode<ETEntityEvents>,
    public ETNode<ETUIAdditiveAnimationTarget> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIElement();
    virtual ~UIElement();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIElement
    void ET_setHostLayout(EntityId newLayoutId) override;
    EntityId ET_getHostLayout() const override;
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
    void ET_setParentHidden(bool flag) override;
    void ET_setParentDisabled(bool flag) override;
    void ET_addAdditiveTransform(const Transform& newAddTm, float newAddAlpha) override;
    void ET_setLayoutPos(const Vec2i& layoutPt) override;

    // ETUIAdditiveAnimationTarget
    void ET_applyAdditiveTranform() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onLoaded() override {}

protected:

    void updateHostLayout();

protected:

    virtual void onZIndexChanged(int newZIndex) = 0;
    virtual void onHide(bool flag) = 0;
    virtual void onAlphaChanged(float newAlpha) = 0;
    virtual void onDisabled(bool flag) = 0;
    virtual void onTransformChanged(const Transform& newTm) = 0;

protected:

    Transform addTm;
    Transform layoutTm;
    EntityId hostLayoutId;
    float alpha;
    float addAlpha;
    int zIndex;
    bool isIgnoringTransform;
    bool isHidden;
    bool isEnabled;
    bool isParentHidden;
    bool isParentDisabled;
    bool isAddTmChanged;
};

#endif /* __UI_ELEMENT_HPP__ */