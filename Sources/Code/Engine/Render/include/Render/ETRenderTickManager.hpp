#ifndef __ET_RENDER_TICK_MANAGER_HPP__
#define __ET_RENDER_TICK_MANAGER_HPP__

struct ETRenderTickManager {
public:

    struct Ticks {
        float gameDt{0.f};
        float uiDt{0.f};
        float renderDt{0.f};
    };

public:

    virtual ~ETRenderTickManager() = default;
    virtual void ET_onRenderTick(float dt) = 0;
    virtual void ET_onGameTick(float dt) = 0;
    virtual void ET_onUITick(float dt) = 0;
    virtual Ticks ET_getTicks() const = 0;
    virtual void ET_fetchDeltaT() = 0;
};

#endif /* __ET_RENDER_TICK_MANAGER_HPP__ */