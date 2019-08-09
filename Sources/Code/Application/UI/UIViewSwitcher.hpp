#ifndef __UI_VIEW_SWITCHER_HPP__
#define __UI_VIEW_SWITCHER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "UI/UIETInterfaces.hpp"
#include "Math/Transform.hpp"

class UIViewSwitcher : public SystemLogic,
    public ETNode<ETTimerEvents>,
    public ETNode<ETUIViewSwitcher> {
public:

    UIViewSwitcher();
    virtual ~UIViewSwitcher();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETTimerEvents
    void ET_onTick(float dt) override;

    // ETUIViewSwitcher
    void ET_swtichView(EntityId newViewId, EntityId oldViewId) override;
    float ET_getSwitchDuration() const override;

private:

    struct SwtichTask {
        Transform startTm;
        EntityId newViewId;
        EntityId oldViewId;
        float duration;
    };

private:

    std::unique_ptr<SwtichTask> activeTask;
    float swtichDuration;
};

#endif /* __UI_VIEW_SWITCHER_HPP__ */