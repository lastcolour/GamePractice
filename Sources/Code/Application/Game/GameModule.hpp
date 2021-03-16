#ifndef __GAME_MODULE_HPP__
#define __GAME_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameModule : public SystemModule {
public:

    GameModule() : SystemModule("Game") {}
    virtual ~GameModule() = default;

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void createSystemConfigs() const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __GAME_MODULE_HPP__ */