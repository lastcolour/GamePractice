#ifndef __ENTITY_LOGICS_REGISTER_HPP__
#define __ENTITY_LOGICS_REGISTER_HPP__

#include "Entity/EntityLogic.hpp"
#include "Reflect/ReflectContext.hpp"

#include <type_traits>

class ClassInfo;

class EntityLogicsRegister {
public:

    EntityLogicsRegister(const char* module);
    ~EntityLogicsRegister();

    template<typename LogicT>
    void registerLogic() {
        static_assert(std::is_base_of<EntityLogic, LogicT>::value,
            "Can't register entity logic if it isn't derived from 'EntityLogic'");
        static_assert(!std::is_abstract<LogicT>::value,
            "Can't register abstract class as an entity logic");
        ReflectContext ctx;
        ctx.reflect<LogicT>();
        registerFromReflectCtx(ctx);
    }

    std::vector<ClassInfo*>& getLogicClasses();
    const char* getModuleName() const;

private:

    void registerFromReflectCtx(ReflectContext& ctx);

private:

    const char* moduleName;
    std::vector<ClassInfo*> logics;
};

#endif /* __ENTITY_LOGICS_REGISTER_HPP__ */