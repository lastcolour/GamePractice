#ifndef __ET_GAME_ELEMS_POOL_HPP__
#define __ET_GAME_ELEMS_POOL_HPP__

#include "Game/ETGameElem.hpp"

struct GameElemPoolInfo {
public:

    static void Reflect(ReflectContext& ctx);

public:

    int weight;
    EBoardElemType elemType;

public:

    GameElemPoolInfo();
};

struct ETGameBoardElemsPool {
    virtual ~ETGameBoardElemsPool() = default;
    virtual EntityId ET_spawnBaseRandomElem() = 0;
    virtual EntityId ET_spawnElem(EBoardElemType elemType) = 0;
    virtual void ET_removeElem(EntityId elemId) = 0;
    virtual void ET_setPoolsSetup(const std::vector<GameElemPoolInfo>& poolsInfo) = 0;
    virtual void ET_setElemEntity(EBoardElemType elemType, const char* entityName) = 0;
};

#endif /* __ET_GAME_ELEMS_POOL_HPP__ */