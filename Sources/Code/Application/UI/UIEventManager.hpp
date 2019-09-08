#ifndef __UI_EVENT_MANAGER_HPP__
#define __UI_EVENT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/ETUIInterfaces.hpp"

#include <unordered_map>
#include <functional>

class UIEventManager : public SystemLogic,
    public ETNode<ETUIEventManager>,
    public ETNode<ETUIViewStackEvents> {

    enum class EViewType {
        Main = 0,
        Game,
        Options,
        About,
        Player,
        EndGame,
        Pause,
        Exit,
        Other
    };

    using EventMapT = std::unordered_map<std::string, std::function<void()>>;
    using ViewMapT = std::unordered_map<std::string, EViewType>;

public:
    UIEventManager();
    virtual ~UIEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIEventManager
    void ET_onEvent(const char* eventName) override;

    // ETUIViewStackEvents
    void ET_onViewStartPush(EntityId viewId) override;
    void ET_onViewFinishPush(EntityId viewId) override;
    void ET_onViewStartPop(EntityId viewId) override;
    void ET_onViewFinishPop(EntityId viewId) override;

private:

    void setupCallbacks();
    void processBackButtonEvent();
    EViewType getViewTypeFromEntityId(EntityId viewId) const;
    EViewType getActiveViewType() const;
    const char* getViewName(EViewType viewType) const;
    void pushView(EViewType viewType);
    void popView();

private:

    EventMapT eventMap;
    ViewMapT viewMap;
};

#endif /* __UI_EVENT_MANAGER_HPP__ */