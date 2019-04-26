#ifndef __GAME_MODULE_HPP__
#define __GAME_MODULE_HPP__

#include "Core/SystemModule.hpp"

class GameModule : public SystemModule {
public:

    GameModule();
    virtual ~GameModule() = default;

protected:

    LogicsContainerPtrT getSystemLogics() const override;
};

#endif /* __GAME_MODULE_HPP__ */