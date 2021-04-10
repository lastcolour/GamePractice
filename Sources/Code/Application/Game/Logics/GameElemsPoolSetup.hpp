#ifndef __GAME_ELEMS_POOL_SETUP_HPP__
#define __GAME_ELEMS_POOL_SETUP_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElemsPool.hpp"

class GameElemsPoolSetup : public EntityLogic,
    public ETNode<ETEntityEvents> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    GameElemsPoolSetup();
    virtual ~GameElemsPoolSetup();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETEntityEvents
    void ET_onTransformChanged(const Transform& newTm) override {}
    void ET_onLoaded() override;

private:

    std::vector<GameElemPoolInfo> elems;
};

#endif /* __GAME_ELEMS_POOL_SETUP_HPP__ */