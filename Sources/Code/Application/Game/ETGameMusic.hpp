#ifndef __ET_GAME_MUSIC_HPP__
#define __ET_GAME_MUSIC_HPP__

enum class EMusicTheme {
    Menu = 0,
    Game,
    None
};

struct ETGameMusic {
    virtual ~ETGameMusic() = default;
    virtual void ET_setTheme(EMusicTheme newTheme) = 0;
};

#endif /* __ET_GAME_MUSIC_HPP__ */