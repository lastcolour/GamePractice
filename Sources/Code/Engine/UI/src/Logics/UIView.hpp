#ifndef __UI_VIEW_HPP__
#define __UI_VIEW_HPP__

#include "Logics/UIBox.hpp"
#include "UI/ETUIView.hpp"

class UIView : public UIBox,
    public ETNode<ETUIView> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIView();
    virtual ~UIView();

    // EntityLogic
    void init() override;

    // ETViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;

    // ETUIView
    void ET_setFocus(bool flag) override;
    bool ET_getFocus() const override;

private:

    bool hasFocus;
};

#endif /* __UI_VIEW_HPP__ */