#ifndef __UI_VIEW_SWITCHER_HPP__
#define __UI_VIEW_SWITCHER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIInterfaces.hpp"
#include "ETApplicationInterfaces.hpp"
#include "Render/ETRenderInterfaces.hpp"

class UIViewSwitcher : public SystemLogic,
    public ETNode<ETUIViewSwitcher>,
    public ETNode<ETTimerEvents>,
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
    void ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) override;
    void ET_swtichView(EntityId newViewId, EntityId oldViewId) override;
    void ET_forceSwtichStop() override;
    void ET_reverse() override;
    float ET_getTotalSwitchDuration() const override;

    // ETRenderEvents
    void ET_onRender(RenderContext& renderCtx) override { (void)renderCtx; }
    void ET_onRenderPortResized() override;

private:

    enum class SwitchState {
        HideOldView = 0,
        ShowNewView,
        Finished
    };

    struct SwitchTask {
        SwitchState state;
        EntityId newViewId;
        EntityId oldViewId;
        float duration;
    };

private:

    SwitchTask switchTask;
};

#endif /* __UI_VIEW_SWITCHER_HPP__ */