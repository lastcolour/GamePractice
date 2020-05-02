#ifndef __ENTITY_LOGIC_REGISTER_HPP__
#define __ENTITY_LOGIC_REGISTER_HPP__

#include "Reflect/ReflectContext.hpp"
#include "EntityLogic.hpp"

#include <type_traits>

class EntityLogicRegister {
public:

    EntityLogicRegister();
    ~EntityLogicRegister();

    template<typename LogicT>
    void registerLogic(const char* logicName) {
        static_assert(std::is_base_of<EntityLogic, LogicT>::value,
            "Can't register entity logic if it isn't derived from 'EntityLogic'");
        ReflectContext ctx;
        if(!ctx.reflect<LogicT>()) {

        } else {

        }
    }
};

#endif /* __ENTITY_LOGIC_REGISTER_HPP__ */