#ifndef __UI_LAYOUT_HPP__
#define __UI_LAYOUT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUILayout.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UILayoutStyle.hpp"

class UILayout : public EntityLogic,
    public ETNode<ETUILayout>,
    public ETNode<ETUIElementEvents>,
    public ETNode<ETUIElemAligner> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILayout();
    virtual ~UILayout();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUILayout
    const UILayoutStyle& ET_getStyle() const override;
    void ET_setStyle(const UILayoutStyle& newStyle) override;
    void ET_addItem(EntityId entityId) override;
    const AABB2D& ET_getCombinedBox() const override;
    std::vector<EntityId> ET_getItems() const override;

    // ETUIElemAligner
    void ET_reAlign() override;

    // ETUIElementEvents
    void ET_onBoxChanged(const AABB2D& newAabb) override {}
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

private:

    AABB2D calculateItem(float& offset, float& prevMargin, EntityId itemId);
    AABB2D calculateAligment(std::vector<AABB2D>& boxes);
    void calculateLayout();

private:

    AABB2D combinedBox;
    int extraZOffset;
    UILayoutStyle style;
    std::vector<EntityId> children;
    bool isCalculatingLayout;
};

#endif /* __UI_LAYOUT_HPP__ */