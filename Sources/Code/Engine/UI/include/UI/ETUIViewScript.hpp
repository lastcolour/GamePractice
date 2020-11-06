#ifndef __ET_UI_VIEW_SCIRPT_HPP__
#define __ET_UI_VIEW_SCIRPT_HPP__

class UIEvent {
public:

    enum class EventType {
        None = 0,
        OnMainViewStartGame,
        OnGameEndViewExit,
        OnBackButton,
        OnSurfaceHidden,
        OnSurfaceShown,
        OnPauseViewResume,
        OnPauseViewRestart,
        OnPauseViewExit,
        OnLevelsStartLevel
    };

public:

    EntityId senderId;
    EventType type;
};

struct ETUIViewScript {
    virtual ~ETUIViewScript() = default;
    virtual void ET_onViewOpened() = 0;
    virtual void ET_onViewClosed() = 0;
    virtual void ET_onViewGetFocus() = 0;
    virtual void ET_onViewLostFocus() = 0;
    virtual void ET_onEvent(const UIEvent& event) = 0;
};

#endif /* __ET_UI_VIEW_SCIRPT_HPP__ */