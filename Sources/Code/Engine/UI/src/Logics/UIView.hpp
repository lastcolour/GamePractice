#ifndef __UI_VIEW_HPP__
#define __UI_VIEW_HPP__

#include "Logics/UIBox.hpp"

class UIView : public UIBox {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIView();
    virtual ~UIView();

    // EntityLogic
    bool init() override;

    // ETViewPortEvents
    void ET_onViewPortChanged(const Vec2i& newSize) override;
};

#endif /* __UI_VIEW_HPP__ */