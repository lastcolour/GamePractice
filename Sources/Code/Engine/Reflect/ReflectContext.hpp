#ifndef __REFLECT_CONTEXT_HPP__
#define __REFLECT_CONTEXT_HPP__

#include "Reflect/ClassInfo.hpp"
#include "Reflect/EnumInfo.hpp"

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
        return registerInfos();
    }

    template<typename ClassT>
    ClassInfo* classInfo(const char* name) {
        auto cInfo = createClassInfo(name, GetTypeId<ClassT>()  );
        if(cInfo) {
            cInfo->template init<ClassT>();
        }
        return cInfo;
    }

    template<typename EnumT>
    EnumInfo* enumInfo(const char* name) {
        return createEnumInfo(name, GetTypeId<EnumT>());
    }

    ClassInfo* getRegisteredClassInfo();

private:

    EnumInfo* createEnumInfo(const char* enumName, TypeId enumTypeId);
    ClassInfo* createClassInfo(const char* className, TypeId classTypeId);
    bool registerInfos();
    bool registerEnums();

private:

    std::unique_ptr<ClassInfo> clsInfo;
    std::vector<std::unique_ptr<EnumInfo>> enumInfos;
    ClassInfo* registeredClsInfo;
};

#endif /* __REFLECT_CONTEXT_HPP__ */