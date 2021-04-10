#include "Game/Logics/GameElemsPoolSetup.hpp"
#include "Game/ETGameElemsPool.hpp"
#include "Game/ETGameBoard.hpp"

void GameElemPoolInfo::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemPoolInfo>("GameElemPoolInfo")) {
        classInfo->addField("elemType", &GameElemPoolInfo::elemType);
        classInfo->addField("weight", &GameElemPoolInfo::weight);
    }
}

GameElemPoolInfo::GameElemPoolInfo() :
    weight(1),
    elemType(EBoardElemType::Blue) {
}

void GameElemsPoolSetup::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<GameElemsPoolSetup>("GameElemsPoolSetup")) {
        classInfo->addField("elems", &GameElemsPoolSetup::elems);
    }
}

GameElemsPoolSetup::GameElemsPoolSetup() {
}

GameElemsPoolSetup::~GameElemsPoolSetup() {
}

void GameElemsPoolSetup::init() {
    ETNode<ETEntityEvents>::connect(getEntityId());
}

void GameElemsPoolSetup::deinit() {
}

void GameElemsPoolSetup::ET_onLoaded() {
    ETNode<ETEntityEvents>::disconnect();
    if(!elems.empty()) {
        ET_SendEvent(&ETGameBoardElemsPool::ET_setPoolsSetup, elems);
        ET_SendEvent(&ETGameBoard::ET_spawnElems);
    }
}