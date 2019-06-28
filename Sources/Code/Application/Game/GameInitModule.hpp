#ifndef __GAME_INIT_MODULE_HPP__
#define __GAME_INIT_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameInitModule : public SystemModule {
public:

    GameInitModule() : SystemModule("GameInit") {}
    virtual ~GameInitModule() = default;

protected:

    LogicsContainerPtrT getSystemLogics() const override;
};

#endif /* __GAME_INIT_MODULE_HPP__ */