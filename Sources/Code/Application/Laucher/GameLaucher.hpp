#ifndef __GAME_LAUCHER_HPP__
#define __GAME_LAUCHER_HPP__

#include "Core/SystemLogic.hpp"
#include "Platform/ETSurface.hpp"

class GameLaucher : public SystemLogic,
    public ETNode<ETSurfaceEvents> {
public:

    GameLaucher();
    virtual ~GameLaucher();

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

    bool isViewExist;
};

#endif /* __GAME_LAUCHER_HPP__ */