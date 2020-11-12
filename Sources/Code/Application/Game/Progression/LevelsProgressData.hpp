#ifndef __LEVELS_PROGRESS_DATA_HPP__
#define __LEVELS_PROGRESS_DATA_HPP__

#include <vector>
#include <string>

class ReflectContext;

class LevelProgress {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelProgress();

public:

    std::string name;
    float completeTime;
    int score;
    int stars;
};

class LevelProgressList {
public:

    static void Reflect(ReflectContext& ctx);

public:

    std::vector<LevelProgress> levels;
};

#endif /* __LEVELS_PROGRESS_DATA_HPP__ */