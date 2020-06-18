#ifndef __GAME_LAUCHER_MODULE_HPP__
#define __GAME_LAUCHER_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameLaucherModule : public SystemModule {
public:

    GameLaucherModule();
    virtual ~GameLaucherModule();

protected:

    LogicsContainerPtrT createSystemLogics() const override;
    void reflectSystemConfigs(ReflectContext& ctx) const override;
    void registerEntityLogics(EntityLogicsRegister& logicsRegister) const override;
};

#endif /* __GAME_LAUCHER_MODULE_HPP__ */