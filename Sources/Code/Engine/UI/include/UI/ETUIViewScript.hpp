#ifndef __ET_UI_VIEW_SCIRPT_HPP__
#define __ET_UI_VIEW_SCIRPT_HPP__

enum class UIEventType {
    None = 0,
    OnMainViewStartGame,
    OnGameEndViewExit,
    OnBackButton,
    OnGameGameEnd,
    OnSurfaceHidden,
    OnSurfaceShown,
    OnPauseViewResume,
    OnPauseViewRestart,
    OnPauseViewExit
};

struct ETUIViewScript {
    virtual ~ETUIViewScript() = default;
    virtual void ET_onGetFocus() = 0;
    virtual void ET_onLostFocus() = 0;
    virtual void ET_onEvent(UIEventType eventType) = 0;
};

#endif /* __ET_UI_VIEW_SCIRPT_HPP__ */