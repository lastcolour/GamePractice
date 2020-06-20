#ifndef __UI_BOX_HPP__
#define __UI_BOX_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/ETRenderCamera.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/UIBoxStyle.hpp"
#include "Core/ETPrimitives.hpp"

class UIBox : public EntityLogic,
    public ETNode<ETUIBox>,
    public ETNode<ETRenderCameraEvents>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBox();
    virtual ~UIBox();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIBox
    const UIBoxStyle& ET_getStyle() const override;
    void ET_setStyle(const UIBoxStyle& newBoxStyle) override;
    const AABB2Di& ET_getBox() const override;
    const UIBoxMargin& ET_getMargin() const override;
    void ET_setLayout(EntityId newLayoutId) override;
    EntityId ET_getLayout() override;

    // ETRenderCameraEvents
    void ET_onRenderPortResized() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override { (void)childId; }

private:

    void calculateBox();
    Vec2i calculateBoxSize(const Vec3& scale);
    UIBoxMargin calculateMargin(const Vec3& scale);

private:

    AABB2Di aabb;
    UIBoxStyle style;
    UIBoxMargin margin;
    EntityId layoutId;
};

#endif /* __UI_BOX_HPP__ */