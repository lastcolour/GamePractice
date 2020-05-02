#ifndef __UI_BASE_BOX_HPP__
#define __UI_BASE_BOX_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "UI/UIStyle.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Entity/ETEntityInterfaces.hpp"

class UIBaseBox : public EntityLogic,
    public ETNode<ETUIBox>,
    public ETNode<ETRenderEvents>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBaseBox();
    virtual ~UIBaseBox();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIBox
    const UIStyle& ET_getStyle() const override;
    void ET_setStyle(const UIStyle& newStyle) override;
    void ET_setCenter(const Vec2i& center) override;
    const AABB2Di& ET_getAabb2di() const override;
    void ET_alignInBox(const AABB2Di& alingBox) override;
    void ET_boxResize() override;
    const Margin& ET_getMaring() const override;
    void ET_show() override;
    void ET_hide() override;
    bool ET_isVisible() const override;
    void ET_disableInteraction() override;
    void ET_enableInteraction() override;
    int ET_getZIndex() const override;
    void ET_setZIndex(int newZIndex) override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override { (void)renderCtx; }
    void ET_onRenderPortResized() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override;

protected:

    virtual Vec2i calculateBoxSize(const AABB2Di& parentBox) const;
    virtual EntityId getRenderId() const;

protected:

    AABB2Di calcBox(const AABB2Di& parentBox) const;
    Vec2i calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const;
    AABB2Di getParentAabb2di() const;
    void setBox(const AABB2Di& newBox);
    void forceResizeFromTop();
    void setUpRenderChild(EntityId renderChildId);

private:

    EntityId getRootUIList() const;
    void syncTransform() const;
    bool isNeedResize();
    Margin calculateMargin(const AABB2Di& parentBox) const;

private:

    UIStyle style;
    Margin margin;
    AABB2Di box;
    AABB2Di lastResizeBox;
    int zIndex;
    bool isVisible;
};

#endif /* __UIBOX_HPP__ */