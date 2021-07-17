#ifndef __ET_GAME_BOARD_SPAWNER_HPP__
#define __ET_GAME_BOARD_SPAWNER_HPP__

struct ETGameBoardSpawner {
    virtual ~ETGameBoardSpawner() = default;
    virtual void ET_setPendingLevel(const std::string& levelName) = 0;
    virtual void ET_loadPendingLevel() = 0;
    virtual const char* ET_getLevelName() const = 0;
    virtual void ET_unloadLevel() = 0;
};

struct ETGameBoardSpawnerEvents {
    virtual ~ETGameBoardSpawnerEvents() = default;
    virtual void ET_onStartLoading() = 0;
    virtual void ET_onStartDestroying() = 0;
};

#endif /* __ET_GAME_BOARD_SPAWNER_HPP__ */