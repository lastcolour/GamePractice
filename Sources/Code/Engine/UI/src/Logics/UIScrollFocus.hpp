#ifndef __UI_SCROLL_FOCUS_HPP__
#define __UI_SCROLL_FOCUS_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUITimer.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIScrollArea.hpp"

class ReflectContext;

class UIScrollFocus : public EntityLogic,
    public ETNode<ETUITimerEvents>,
    public ETNode<ETUIScrollFocus> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIScrollFocus();
    virtual ~UIScrollFocus();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUITimerEvents
    void ET_onUITick(float dt) override;

    // ETUIScrollFocus
    void ET_setFocusToEntity(EntityId newFocusEntId) override;
    void ET_setPendingFocus() override;

private:

    void applyFocusToEntity();

private:

    EntityId focusEntId;
};

#endif /* __UI_SCROLL_FOCUS_HPP__ */