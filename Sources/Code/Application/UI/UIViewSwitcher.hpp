#ifndef __UI_VIEW_SWITCHER_HPP__
#define __UI_VIEW_SWITCHER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "Math/Transform.hpp"
#include "Render/ETRenderInterfaces.hpp"

class UIViewSwitcher : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETUIViewSwitcher>,
    public ETNode<ETRenderEvents> {
public:

    UIViewSwitcher();
    virtual ~UIViewSwitcher();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETUIViewSwitcher
    void ET_forceSwtichStop() override;
    void ET_swtichView(EntityId newViewId, EntityId oldViewId) override;
    void ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) override;
    void ET_reverse() override;
    float ET_getSwitchDuration() const override;

    // ETRenderEvents
    void ET_onRender(const RenderContext& renderCtx) override { (void)renderCtx; }
    void ET_onRenderPortResized() override;

private:

    struct SwtichTask {
        Transform startTm;
        EntityId newViewId;
        EntityId oldViewId;
        float duration;
        bool reverse;
    };

private:

    void setupTask(EntityId newViewId, EntityId oldViewId, bool reverse);

private:

    std::unique_ptr<SwtichTask> activeTask;
    float swtichDuration;
};

#endif /* __UI_VIEW_SWITCHER_HPP__ */