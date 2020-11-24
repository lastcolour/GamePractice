#ifndef __UI_ANIMATION_FRAME_HPP__
#define __UI_ANIMATION_FRAME_HPP__

#include "Core/Core.hpp"
#include "Math/Transform.hpp"

class ReflectContext;

enum class EAnimLerpMode {
    Linear = 0,
    Pow_2,
    Pow_4,
    InvPow_2,
    InvPow_4,
    Sin,
    Sin_Sqr_2,
    InvSin_Sqr_2
};

enum class EShowEvent {
    None = 0,
    Show,
    Hide
};

enum class EAnimFrameState {
    Pending = 0,
    InProgress,
    Finished
};

class UIAnimationFrame {
public:

    static void Reflect(ReflectContext& ctx);

public:

    UIAnimationFrame();

public:

    Vec2 offset;
    Vec2 scale;
    float duration;
    float alpha;
    EAnimLerpMode lerpMode;
    EShowEvent onStartEvent;
    EShowEvent onEndEvent;
    EAnimFrameState state;
};

#endif /* __UI_ANIMATION_FRAME_HPP__ */