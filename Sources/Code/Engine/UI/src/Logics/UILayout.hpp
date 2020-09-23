#ifndef __UI_LAYOUT_HPP__
#define __UI_LAYOUT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UILayoutStyle.hpp"
#include "Core/ETPrimitives.hpp"

class UILayout : public EntityLogic,
    public ETNode<ETUILayout>,
    public ETNode<ETUIElementEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UILayout();
    virtual ~UILayout();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUILayout
    const UILayoutStyle& ET_getStyle() const override;
    void ET_setStyle(const UILayoutStyle& newStyle) override;
    void ET_addItem(EntityId entityId) override;
    void ET_update() override;
    void ET_setHostBox(EntityId entityId) override;
    const AABB2Di& ET_getCombinedBox() const override;

    // ETUIElementEvents
    void ET_onBoxResized(const AABB2Di& newAabb) override;
    void ET_onZIndexChanged(int newZIndex) override;
    void ET_onAlphaChanged(float newAlpha) override;
    void ET_onHidden(bool flag) override;
    void ET_onDisabled(bool flag) override;
    void ET_onIngoreTransform(bool flag) override;

private:

    AABB2Di calculateItem(int& offset, int& prevMargin, EntityId itemId);
    void calculateAligment(std::vector<AABB2Di>& boxes);
    void calculateLayout();

private:

    EntityId hostBoxId;
    AABB2Di combinedBox;
    UILayoutStyle style;
    std::vector<EntityId> children;
    bool isCalculatingLayout;
};

#endif /* __UI_LAYOUT_HPP__ */