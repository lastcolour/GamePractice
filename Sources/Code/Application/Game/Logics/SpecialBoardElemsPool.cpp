#include "Game/Logics/SpecialBoardElemsPool.hpp"

#include <cassert>

void SpecialBoardElemsPool::Reflect(ReflectContext& ctx) {
    if(auto classInfo = ctx.classInfo<SpecialBoardElemsPool>("SpecialBoardElemsPool")) {
        classInfo->addResourceField("vRocketEntity", ResourceType::Entity,
            &SpecialBoardElemsPool::vRocketEntity);
        classInfo->addResourceField("hRocketEntity", ResourceType::Entity,
            &SpecialBoardElemsPool::hRocketEntity);
        classInfo->addResourceField("bombEntity", ResourceType::Entity,
            &SpecialBoardElemsPool::bombEntity);
        classInfo->addResourceField("starEntity", ResourceType::Entity,
            &SpecialBoardElemsPool::starEntity);
    }
}

SpecialBoardElemsPool::SpecialBoardElemsPool() {
}

SpecialBoardElemsPool::~SpecialBoardElemsPool() {
}

void SpecialBoardElemsPool::init() {
    ETNode<ETSpecialBoardElemsPool>::connect(getEntityId());
}

void SpecialBoardElemsPool::deinit() {
}

EntityId SpecialBoardElemsPool::ET_createSpecialElem(EBoardElemType elemType) {
    switch(elemType) {
        case EBoardElemType::HRocket: {
            break;
        }
        case EBoardElemType::VRocket: {
            break;
        }
        case EBoardElemType::Bomb: {
            break;
        }
        case EBoardElemType::Star: {
            break;
        }
        default: {
            assert(false && "Invalid elem type");
            return InvalidEntityId;
        }
    }
    return InvalidEntityId;
}