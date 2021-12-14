#ifndef __UI_SCROLL_FOCUS_HPP__
#define __UI_SCROLL_FOCUS_HPP__

#include "Entity/EntityLogic.hpp"
#include "UI/ETUITimer.hpp"
#include "UI/ETUIScrollArea.hpp"

class UIScrollFocus : public EntityLogic,
    public ETNode<ETUIScrollFocus> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIScrollFocus();
    virtual ~UIScrollFocus();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETUIScrollFocus
    void ET_setFocusToEntity(EntityId newFocusEntId) override;
    void ET_setPendingFocus() override;

private:

    void applyFocusToEntity();

private:

    EntityId focusEntId;
};

#endif /* __UI_SCROLL_FOCUS_HPP__ */