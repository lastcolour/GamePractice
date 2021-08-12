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
    void ET_onGameTick(float dt) override;
    void ET_onUITick(float dt) override;
    float ET_getGameDeltaT() const override;
    float ET_getUIDeltaT() const override;
    void ET_fetchDeltaT() override;

private:

    struct TickData {
        float gameDt {0.f};
        float uiDt {0.f};
    };

private:

    std::mutex mutex;
    TickData prev;
    TickData curr;
};

#endif /* __RENDER_TICK_MANAGER_HPP__*/