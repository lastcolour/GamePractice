#ifndef __ET_GAME_ELEMS_POOL_HPP__
#define __ET_GAME_ELEMS_POOL_HPP__

#include "Game/ETGameElem.hpp"

class ElementDescriptor;

struct ETGameBoardElemsPool {
    virtual ~ETGameBoardElemsPool() = default;
    virtual EntityId ET_spawnElem() = 0;
    virtual void ET_removeElem(EntityId elemId) = 0;
    virtual void ET_addElemsToSpawn(const ElementDescriptor& newElemDescr) = 0;
};

struct ETSpecialBoardElemsPool {
    virtual ~ETSpecialBoardElemsPool() = default;
    virtual EntityId ET_createSpecialElem(EBoardElemType elemType) = 0;
};

#endif /* __ET_GAME_ELEMS_POOL_HPP__ */