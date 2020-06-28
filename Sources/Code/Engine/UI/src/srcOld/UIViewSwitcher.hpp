#ifndef __UI_VIEW_SWITCHER_HPP__
#define __UI_VIEW_SWITCHER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIView.hpp"
#include "Core/ETTimer.hpp"
#include "Core/ETPrimitives.hpp"
#include "Render/ETRenderCamera.hpp"

class UIViewSwitcher : public SystemLogic,
    public ETNode<ETUIViewSwitcher>,
    public ETNode<ETAppTimerEvents>,
    public ETNode<ETRenderCameraEvents> {
public:

    UIViewSwitcher();
    virtual ~UIViewSwitcher();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETAppTimerEvents
    void ET_onAppTick(float dt) override;

    // ETUIViewSwitcher
    void ET_reverseSwitchView(EntityId newViewId, EntityId oldViewId) override;
    void ET_swtichView(EntityId newViewId, EntityId oldViewId) override;
    void ET_forceSwtichStop() override;
    void ET_reverse() override;
    float ET_getTotalSwitchDuration() const override;

    // ETRenderCameraEvents
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