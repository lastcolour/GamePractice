#ifndef __SPECIAL_BOARD_ELEMS_POOL_HPP__
#define __SPECIAL_BOARD_ELEMS_POOL_HPP__

#include "Entity/EntityLogic.hpp"
#include "Game/ETGameElemsPool.hpp"

class SpecialBoardElemsPool : public EntityLogic,
    public ETNode<ETSpecialBoardElemsPool> {
public:

    static void Reflect(ReflectContext& ctx);

public:

    SpecialBoardElemsPool();
    virtual ~SpecialBoardElemsPool();

    // EntityLogic
    void init() override;
    void deinit() override;

    // ETSpecialBoardElemsPool
    EntityId ET_createSpecialElem(EBoardElemType elemType) override;

private:

    std::string vRocketEntity;
    std::string hRocketEntity;
    std::string bombEntity;
    std::string starEntity;
};

#endif /* __SPECIAL_BOARD_ELEMS_POOL_HPP__ */