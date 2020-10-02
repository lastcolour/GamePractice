#ifndef __LEVEL_SELECTOR_LOGIC_HPP__
#define __LEVEL_SELECTOR_LOGIC_HPP__

#include "Entity/EntityLogic.hpp"
#include "Core/ETPrimitives.hpp"
#include "Game/ETLevelSelector.hpp"

class ReflectContext;

class LevelSelectorLogic : public EntityLogic,
    public ETNode<ETLevelSelector> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    LevelSelectorLogic();
    virtual ~LevelSelectorLogic();

    // EntityLogic
    bool init() override;
    void deinit() override;

    // ETLevelSelector
    const char* ET_getLevelName() const override;

private:

    void setLevelName(const char* newLevelName);

private:

    std::string levelName;
};

#endif /* __LEVEL_SELECTOR_LOGIC_HPP__ */