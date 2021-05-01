#ifndef __ET_RENDER_TICK_MANAGER_HPP__
#define __ET_RENDER_TICK_MANAGER_HPP__

struct ETRenderTickManager {
    virtual ~ETRenderTickManager() = default;
    virtual void ET_onGameTick(float dt) = 0;
    virtual void ET_onUITick(float dt) = 0;
    virtual float ET_getGameDeltaT() const = 0;
    virtual float ET_getUIDeltaT() const = 0;
    virtual void ET_fetchDeltaT() = 0;
};

#endif /* __ET_RENDER_TICK_MANAGER_HPP__ */