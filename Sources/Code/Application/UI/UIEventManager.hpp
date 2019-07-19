#ifndef __UI_EVENT_MANAGER_HPP__
#define __UI_EVENT_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "UI/UIETInterfaces.hpp"

#include <unordered_map>

enum class EUIEventType;

class UIEventManager : public SystemLogic,
    public ETNode<ETUIEventManager> {

    using EventMapT = std::unordered_map<std::string, EUIEventType>;

public:
    UIEventManager();
    virtual ~UIEventManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETUIEventManager
    void ET_onEvent(const std::string& eventName) override;

private:

    void processEvent(EUIEventType eventType);

private:

    EventMapT eventMap;
};

#endif /* __UI_EVENT_MANAGER_HPP__ */