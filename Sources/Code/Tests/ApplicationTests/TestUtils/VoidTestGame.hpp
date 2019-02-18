#ifndef __VOID_TEST_GAME_HPP__
#define __VOID_TEST_GAME_HPP__

#include "Game/Game.hpp"

#include <mutex>

class VoidTestGame : public Game {
public:

    VoidTestGame() = default;
    virtual ~VoidTestGame() = default;

    bool init() {
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_init;
        }
        return res;
    }
    bool shouldRun() { 
        bool res = false;
        {
            std::lock_guard<std::mutex> lock(mutex);
            res = retRes_shouldRun;
        }
        return res;
    }
    void update() {
        std::lock_guard<std::mutex> lock(mutex);
        ++callCount_update;
    }

public:

    std::mutex mutex;
    bool retRes_init { true };
    bool retRes_shouldRun { true };
    size_t callCount_update;
};

#endif /* __VOID_TEST_GAME_HPP__ */