#ifndef __RENDER_TICK_MANAGER_HPP__
#define __RENDER_TICK_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Render/ETRenderTickManager.hpp"

#include <mutex>

class RenderTickManager : public SystemLogic,
    public ETNode<ETRenderTickManager> {
public:

    RenderTickManager();
    virtual ~RenderTickManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETRenderTickManager
    void ET_onRenderTick(float dt) override;
    void ET_onGameTick(float dt) override;
    void ET_onUITick(float dt) override;
    Ticks ET_getTicks() const override;
    void ET_fetchDeltaT() override;

private:

    std::mutex mutex;
    Ticks prev;
    Ticks curr;
};

#endif /* __RENDER_TICK_MANAGER_HPP__*/