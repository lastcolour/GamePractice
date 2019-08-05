#ifndef __UI_BASE_BOX_HPP__
#define __UI_BASE_BOX_HPP__

#include "Game/GameLogic.hpp"
#include "UI/UIETInterfaces.hpp"
#include "UI/UIStyle.hpp"
#include "Render/ETRenderInterfaces.hpp"
#include "Game/ETGameInterfaces.hpp"

class UIBaseBox : public GameLogic,
    public ETNode<ETUIBox>,
    public ETNode<ETRenderEvents>,
    public ETNode<ETGameObjectEvents> {
public:

    UIBaseBox();
    virtual ~UIBaseBox();

    // GameLogic
    bool serialize(const JSONNode& node) override;
    bool init() override;

    // ETUIBox
    const UIStyle& ET_getStyle() const override;
    void ET_setStyle(const UIStyle& newStyle) override;
    void ET_setCenter(const Vec2i& center) override;
    const AABB2Di& ET_getAabb2di() const override;
    void ET_alignInBox(const AABB2Di& alingBox) override;
    void ET_boxResize() override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override { (void)renderCtx; }
    void ET_onRenderPortResized() override;

    // ETGameObjectEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override;

protected:

    virtual Vec2i calculateBoxSize(const AABB2Di& parentBox) const;

protected:

    AABB2Di calcBox(const AABB2Di& parentBox) const;
    Vec2i calcCenter(const AABB2Di& selfBox, const AABB2Di& parentBox) const;
    AABB2Di getParentAabb2di() const;
    void setBox(const AABB2Di& newBox);
    EntityId getRootUIBox() const;

private:

    void syncTransform() const;

private:

    UIStyle style;
    AABB2Di box;
};

#endif /* __UIBOX_HPP__ */