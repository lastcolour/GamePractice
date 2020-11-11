#ifndef __LEVEL_PROGRESSION_HPP__
#define __LEVEL_PROGRESSION_HPP__

#include "Core/SystemLogic.hpp"

class LevelProgression : public SystemLogic {
public:

    LevelProgression();
    virtual ~LevelProgression();

    // SystemLogic
    bool init() override;
    void deinit() override;

private:

    struct LevelProgress {
        int starsCount;
        float time;
    };

private:

    std::vector<LevelProgress> progress;
};

#endif /* __LEVEL_PROGRESSION_HPP__ */