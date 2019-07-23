#ifndef __UIBOX_HPP__
#define __UIBOX_HPP__

#include "Game/GameLogic.hpp"
#include "UI/UIETInterfaces.hpp"
#include "UI/UIStyle.hpp"
#include "Render/ETRenderInterfaces.hpp"

class UIBox : public GameLogic,
    public ETNode<ETUIBox>,
    public ETNode<ETRenderEvents> {
public:

    UIBox();
    virtual ~UIBox();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    const AABB2Di& ET_getAabb2di() const override;
    void ET_setCenter(const Vec2i& center) override;
    void ET_alignInBox(const AABB2Di& alingBox) override;
    void ET_boxResize() override;
    const UIStyle& ET_getStyle() const override;
    void ET_setStyle(const UIStyle& newStyle) override;
    void ET_addChildElement(EntityId childId) override;
    void ET_boxResizeInside(const AABB2Di& resizeBox) override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override { (void)renderCtx; }
    void ET_onRenderPortResized() override;

protected:

    AABB2Di calcBox(const AABB2Di& parentBox) const;
    Vec2i calcSize(const AABB2Di& parentBox) const;
    Vec2i calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const;
    AABB2Di getParentAaabb2di() const;
    void setBox(const AABB2Di& newBox);
    EntityId getRendererId() const;

private:

    void syncTransform() const;
    bool createRenderer();
    void updateRendererParams();
    EntityId getRootUIBox() const;

private:

    UIStyle style;
    AABB2Di box;
    EntityId renderId;
};

#endif /* __UIBOX_HPP__ */