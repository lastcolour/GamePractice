#ifndef __REFLECT_CONTEXT_HPP__
#define __REFLECT_CONTEXhT_HPP__

#include "Reflect/ClassInfo.hpp"

class ReflectContext {
public:

    ReflectContext();
    ~ReflectContext();

    bool reflectEmbedded(const std::function<void(ReflectContext&)>& reflectFunc);

    template<typename ClassT>
    bool reflect() {
        ClassT::reflect(*this);
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

    bool registerClassInfo();

private:

    std::unique_ptr<ClassInfo> clsInfo;
    ClassInfo* registeredClsInfo;
};

#endif /* __REFLECT_CONTEXT_HPP__ */