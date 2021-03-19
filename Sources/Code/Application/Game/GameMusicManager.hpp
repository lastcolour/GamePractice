#ifndef __GAME_MUSIC_MANAGER_HPP__
#define __GAME_MUSIC_MANAGER_HPP__

#include "Core/SystemLogic.hpp"
#include "Audio/Sound.hpp"
#include "ETGameMusic.hpp"

class GameSongNode {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameSongNode();

public:

    Sound sound;
    float weight;
};

class GameMusicManager : public SystemLogic,
    public ETNode<ETGameMusic> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameMusicManager();
    virtual ~GameMusicManager();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETGameMusic
    void ET_setTheme(EMusicTheme newTheme);

private:

    std::vector<GameSongNode> mainSongs;
    std::vector<GameSongNode> levelsSongs;
    std::vector<GameSongNode> gameSongs;
};

#endif /* __GAME_MUSIC_MANAGER_HPP__ */