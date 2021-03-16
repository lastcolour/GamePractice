#ifndef __GLOBAL_DATA_HPP__
#define __GLOBAL_DATA_HPP__

#include "Core/GlobalEnvironment.hpp"
#include "Reflect/ReflectContext.hpp"

class GlobalData {
public:

    GlobalData();
    ~GlobalData();

    template<typename T>
    void create() {
        ReflectContext ctx;
        ctx.reflect<T>();
        create(ctx.getRegisteredClassInfo());
    }

    template<typename T>
    void createAndLoad(const char* fileName) {
        ReflectContext ctx;
        ctx.reflect<T>();
        createAndLoad(ctx.getRegisteredClassInfo(), fileName);
    }

    template<typename T>
    T* get() {
        auto typeId = GetTypeId<T>();
        return reinterpret_cast<T*>(getByTypeId(typeId));
    }

private:

    void createAndLoad(ClassInfo* classInfo, const char* fileName);
    void create(ClassInfo* classInfo);
    void* getByTypeId(TypeId typeId);

private:

    std::unordered_map<TypeId, ClassInstance> data;
};

template<typename T>
void CreateGlobal() {
    GetEnv()->GetGlobalData()->create<T>();
}

template<typename T>
void CreateGlobal(const char* configFile) {
    GetEnv()->GetGlobalData()->createAndLoad<T>(configFile);
}

template<typename T>
T* GetGlobal() {
    return GetEnv()->GetGlobalData()->get<T>();
}

#endif /* __GLOBAL_DATA_HPP__ */