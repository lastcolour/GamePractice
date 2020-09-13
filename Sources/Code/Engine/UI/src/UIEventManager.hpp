#ifndef __UI_EVENT_MANGER_HPP__
#define __UI_EVENT_MANGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIButton.hpp"
#include "UI/ETUIView.hpp"
#include "Core/ETPrimitives.hpp"

#include <unordered_map>
#include <functional>

class UIEventManager : public SystemLogic,
    public ETNode<ETUIButtonEventManager>,
    public ETNode<ETUIEventManager>,
    public ETNode<ETUIViewAppearAnimationEvents> {
public:

    UIEventManager();
    virtual ~UIEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIButtonEventManager
    EntityId ET_getActiveButton() const override;
    void ET_setActiveButton(EntityId buttonId) override;

    // ETUIViewAppearAnimationEvents
    void ET_onViewAppeared(EntityId viewId) override;
    void ET_onViewDisappeared(EntityId viewId) override {}

    // ETUIEventManager
    void ET_onEvent(UIEventType eventType) override;

private:

    void initEventHandlers();
    void handleBackButtonEvent(UIViewType activeViewType);

private:

    EntityId activeButtonId;
    std::unordered_map<UIEventType, std::function<void(UIViewType)>> eventHandlers;
};

#endif /* __UI_EVENT_MANGER_HPP__ */