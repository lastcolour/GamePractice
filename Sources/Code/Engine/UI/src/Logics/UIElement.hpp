#ifndef __UI_ELEMENT_HPP__
#define __UI_ELEMENT_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUIBox.hpp"
#include "Core/ETPrimitives.hpp"
#include "Entity/ETEntity.hpp"

class UIElement : public EntityLogic,
    public ETNode<ETUIElement>,
    public ETNode<ETEntityEvents> {
public:

    UIElement();
    virtual ~UIElement();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETUIElement
    void ET_setLayout(EntityId newLayoutId) override;
    EntityId ET_getLayout() override;
    void ET_setZIndex(int newZIndex) override;
    int ET_getZIndex() const override;
    int ET_getZIndexDepth() const override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) {}
    void ET_onChildAdded(EntityId childId) override;

protected:

    void updateLayout();

protected:

    virtual void onZIndexChanged(int newZIndex) = 0;

protected:

    EntityId layoutId;
    int zIndex;
};

#endif /* __UI_ELEMENT_HPP__ */