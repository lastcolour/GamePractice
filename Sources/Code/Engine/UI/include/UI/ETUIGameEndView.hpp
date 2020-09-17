#ifndef __ET_UI_GAME_END_VIEW_HPP__
#define __ET_UI_GAME_END_VIEW_HPP__

struct ETUIStarsAppearAnimation {
    virtual ~ETUIStarsAppearAnimation() = default;
    virtual void ET_showStars(int starsCount) = 0;
};

struct ETUIStarsAppearAnimationEvents {
    virtual ~ETUIStarsAppearAnimationEvents() = default;
    virtual void ET_onStarsAppeared() = 0;
};

#endif /* __ET_UI_GAME_END_VIEW_HPP__ */