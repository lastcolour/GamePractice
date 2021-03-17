#ifndef __GLOBAL_DATA_HPP__
#define __GLOBAL_DATA_HPP__

#include "Core/GlobalEnvironment.hpp"
#include "Core/TypeId.hpp"
#include "Reflect/ReflectUtils.hpp"
#include "Reflect/ReflectContext.hpp"

class GlobalData {
public:

    GlobalData();
    ~GlobalData();

    template<typename T>
    void create() {
        create(ReflectUtils::GetOrCreateClassInfo<T>());
    }

    template<typename T>
    void createAndLoad(const char* fileName) {
        createAndLoad(ReflectUtils::GetOrCreateClassInfo<T>(), fileName);
    }

    template<typename T>
    void remove() {
        auto typeId = GetTypeId<T>();
        removeByTypeId(typeId);
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
    void removeByTypeId(TypeId typeId);

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
void RemoveGlobal() {
    GetEnv()->GetGlobalData()->remove<T>();
}

template<typename T>
T* GetGlobal() {
    return GetEnv()->GetGlobalData()->get<T>();
}

#endif /* __GLOBAL_DATA_HPP__ */