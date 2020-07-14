#ifndef __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__
#define __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

class ReflectContext;

struct UIViewAppearAnimationElement {
public:

    static void Reflect(ReflectContext& ctx);

public: 

    UIViewAppearAnimationElement();
    ~UIViewAppearAnimationElement();

public:

    EntityId elemId;
    Transform origTm;
    float startDelay;
    float duration;
    float xMoveOffset;
    float yMoveOffset;
    float startScale;
    float startAlpha;
    bool isHidded;
};

#endif /* __UI_VIEW_APPEAR_ANIMATION_ELEMENT_HPP__ */