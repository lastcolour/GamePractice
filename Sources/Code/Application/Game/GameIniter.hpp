#ifndef __GAME_INITER_HPP__
#define __GAME_INITER_HPP__

#include "Core/SystemLogic.hpp"
#include "ETApplicationInterfaces.hpp"
#include "UI/ETUIInterfaces.hpp"

class GameIniter : public SystemLogic,
    ETNode<ETSurfaceEvents> {
public:

    GameIniter();
    virtual ~GameIniter();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETSurfaceEvents
    void ET_onSurfaceCreated() override;
    void ET_onSurfaceDestroyed() override;
    void ET_onSurfaceHidden() override {}
    void ET_onSurfaceShown() override {}
    void ET_onSurfaceLostFocus() override {}
    void ET_onSurfaceGainFocus() override {}
    void ET_onSurfaceResized(const Vec2i& size) override { (void)size; }

private:

    void openMainView();

private:

    EntityId mainViewId;
};

#endif /* __GAME_INITER_HPP__ */