#ifndef __UI_LAYOUT_HPP__
#define __UI_LAYOUT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "UI/UILayoutStyle.hpp"
#include "Core/ETPrimitives.hpp"

class UILayout : public EntityLogic,
    public ETNode<ETUILayout> {
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
    void ET_setIgnoreUpdates(bool flag) override;
    std::vector<EntityId> ET_getItems() const override;

private:

    AABB2Di calculateItem(int& offset, int& prevMargin, EntityId itemId);
    Vec2i calcAligmentCenter(AABB2Di& parentBox, AABB2Di& box);
    void calculateAligment(std::vector<AABB2Di>& boxes);
    void calculateLayout();

private:

    UILayoutStyle style;
    std::vector<EntityId> children;
    bool ingoreUpdates;
};

#endif /* __UI_LAYOUT_HPP__ */