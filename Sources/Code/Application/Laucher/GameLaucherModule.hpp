#ifndef __GAME_LAUCHER_MODULE_HPP__
#define __GAME_LAUCHER_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameLaucherModule : public SystemModule {
public:

    GameLaucherModule() : SystemModule("GameLaucher") {}
    virtual ~GameLaucherModule() = default;

protected:

    ConfigsPtrT getSystemConfigs() const override;
    LogicsContainerPtrT getSystemLogics() const override;
    void registerEntityLogics(EntityLogicRegister& logicRegister) const override;

};

#endif /* __GAME_LAUCHER_MODULE_HPP__ */