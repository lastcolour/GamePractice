#ifndef __UI_LAYOUT_HPP__
#define __UI_LAYOUT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UILayoutStyle.hpp"
#include "Core/ETPrimitives.hpp"

class UILayout : public EntityLogic,
    public ETNode<ETUILayout>,
    public ETNode<ETUIBoxEvents> {
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

    // ETUIBoxEvents
    void ET_onBoxResized(const AABB2Di& newAAbb) override;

private:

    AABB2Di calculateItem(Vec2i& offset, Vec2i& prevMargin, EntityId itemId);
    Vec2i calcAligmentCenter(AABB2Di& parentBox, AABB2Di& box);
    void calculateAligment(std::vector<AABB2Di>& boxes);
    void calculateLayout();

private:

    UILayoutStyle style;
    std::vector<EntityId> children;
};

#endif /* __UI_LAYOUT_HPP__ */