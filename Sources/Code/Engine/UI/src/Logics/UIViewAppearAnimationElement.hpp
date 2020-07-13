#ifndef __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__
#define __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__

#include "Core/Core.hpp"

class ReflectContext;

enum class AppearType {
    None = 0,
    FromTop
};

struct UIViewAppearAnimationElement {
public:

    static void Reflect(ReflectContext& ctx);

public: 

    UIViewAppearAnimationElement();
    ~UIViewAppearAnimationElement();

public:

    EntityId elemId;
    float startDelay;
    float moveOffset;
    float duration;
    AppearType appearType;
};

#endif /* __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__ */