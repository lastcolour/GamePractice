#ifndef __LEVEL_PROGRESSION_SYSTEM_HPP__
#define __LEVEL_PROGRESSION_SYSTEM_HPP__

#include "Core/SystemLogic.hpp"
#include "Game/Progression/LevelsProgressData.hpp"
#include "Game/ETLevelProgress.hpp"

class LevelProgressionSystem : public SystemLogic,
    public ETNode<ETLevelsProgression> {
public:

    LevelProgressionSystem();
    virtual ~LevelProgressionSystem();

    // SystemLogic
    bool init() override;
    void deinit() override;

    // ETLevelsProgression
    const LevelProgress* ET_getLevelProgress(const char* levelName) const override;
    void ET_setLevelProgress(const LevelProgress& newLevelProgress) override;
    const LevelProgressDelta* ET_getProgressDelta() const override;
    void ET_resetProgressDelta() override;

private:

    std::unique_ptr<LevelProgressDelta> progressDelta;
    LevelProgressList levelsData;
};

#endif /* __LEVEL_PROGRESSION_SYSTEM_HPP__ */