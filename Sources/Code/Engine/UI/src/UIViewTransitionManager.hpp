#ifndef __UI_VIEW_TRANSITION_MANAGER_HPP__
#define __UI_VIEW_TRANSITION_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "UI/ETUIAnimation.hpp"
#include "UI/ETUIView.hpp"

#include <vector>

class UIViewTransitionManager : public SystemLogic,
    public ETNode<ETUIAnimationSequenceEvent>,
    public ETNode<ETUIViewTransitionManager> {
public:

    UIViewTransitionManager();
    virtual ~UIViewTransitionManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIAnimationSequenceEvent
    void ET_onAnimationPlayed(EntityId sourceId, EAnimSequenceType animType) override;

    // ETUIViewTransitionManager
    bool ET_hasActiveTransition() const override;

    // ETUIViewTransitionManager
    void ET_addAppearing(EntityId viewId) override;
    void ET_addDisappearing(EntityId viewId) override;

private:

    struct AppearTask {
        EntityId viewId;
        bool isAppearing;
    };

private:

    void startNextTask();

private:

    std::vector<AppearTask> tasks;
};

#endif /* __UI_VIEW_TRANSITION_MANAGER_HPP__ */