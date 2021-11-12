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
        return registerInfos();
    }

    template<typename ClassT>
    Reflect::ClassInfo* classInfo(const char* name) {
        auto cInfo = createClassInfo(name, Core::GetTypeId<ClassT>()  );
        if(cInfo) {
            cInfo->template init<ClassT>();
        }
        return cInfo;
    }

    template<typename EnumT>
    Reflect::EnumInfo* enumInfo(const char* name) {
        return createEnumInfo(name, Core::GetTypeId<EnumT>());
    }

    Reflect::ClassInfo* getRegisteredClassInfo();

private:

    Reflect::EnumInfo* createEnumInfo(const char* enumName, Core::TypeId enumTypeId);
    Reflect::ClassInfo* createClassInfo(const char* className, Core::TypeId classTypeId);
    bool registerInfos();
    bool registerEnums();

private:

    std::unique_ptr<Reflect::ClassInfo> clsInfo;
    std::vector<std::unique_ptr<Reflect::EnumInfo>> enumInfos;
    Reflect::ClassInfo* registeredClsInfo;
};

#endif /* __REFLECT_CONTEXT_HPP__ */