#ifndef __GAME_MODULE_HPP__
#define __GAME_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameModule : public SystemModule {
public:

    GameModule() : SystemModule("Game") {}
    virtual ~GameModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;
};

#endif /* __GAME_MODULE_HPP__ */