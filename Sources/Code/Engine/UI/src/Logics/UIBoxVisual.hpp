#ifndef __UI_BOX_VISUAL_HPP__
#define __UI_BOX_VISUAL_HPP__

#include "Entity/EntityLogic.hpp"
#include "Entity/ETEntity.hpp"
#include "UI/ETUIBox.hpp"
#include "Render/Color.hpp"
#include "Core/ETPrimitives.hpp"

class UIBoxVisual : public EntityLogic,
    public ETNode<ETUIBoxEvents>,
    public ETNode<ETUIVisibleElement>,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIBoxVisual();
    virtual ~UIBoxVisual();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIVisibleElement
    void ET_show() override;
    void ET_hide() override;
    bool ET_isVisible() const override;
    void ET_setZIndex(int newZIndex) override;

    // ETUIBoxEvents
    void ET_onBoxResized() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override;
    void ET_onChildAdded(EntityId childId) override { (void)childId; }

protected:

    EntityId renderId;
};

#endif /* __UI_BOX_VISUAL_HPP__ */