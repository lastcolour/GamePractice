#ifndef __REFLECT_CONTEXT_HPP__
#define __REFLECT_CONTEXT_HPP__

#include "Reflect/ClassInfo.hpp"

class ReflectContext {
public:

    ReflectContext();
    ~ReflectContext();

    bool reflectEmbedded(const std::function<void(ReflectContext&)>& reflectFunc);

    template<typename ClassT>
    bool reflect() {
        static_assert(std::is_function<decltype(ClassT::Reflect)>::value
            && std::is_same<decltype(&ClassT::Reflect), void(*)(ReflectContext&)>::value,
            "Type does not provide required T::Reflect(ReflectContext&) declaration");
        ClassT::Reflect(*this);
        return registerClassInfo();
    }

    template<typename ClassT>
    ClassInfo* classInfo(const char* name) {
        if(clsInfo) {
            return nullptr;
        }
        clsInfo.reset(new ClassInfo);
        if(!clsInfo) {
            return nullptr;
        }
        if(!clsInfo->init<ClassT>(name)) {
            return nullptr;
        }
        return clsInfo.get();
    }

    ClassInfo* getRegisteredClassInfo();

private:

    bool createClassInfo(const char* className);
    bool registerClassInfo();

private:

    std::unique_ptr<ClassInfo> clsInfo;
    ClassInfo* registeredClsInfo;
};

#endif /* __REFLECT_CONTEXT_HPP__ */