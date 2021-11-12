#ifndef __GLOBAL_DATA_HPP__
#define __GLOBAL_DATA_HPP__

#include "Reflect/ReflectUtils.hpp"

namespace Core {

class GlobalData {
public:

    GlobalData();
    ~GlobalData();

    template<typename T>
    void create() {
        create(Reflect::GetOrCreateClassInfo<T>());
    }

    template<typename T>
    void createAndLoad(const char* fileName) {
        createAndLoad(Reflect::GetOrCreateClassInfo<T>(), fileName);
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

    void createAndLoad(Reflect::ClassInfo* classInfo, const char* fileName);
    void create(Reflect::ClassInfo* classInfo);
    void* getByTypeId(TypeId typeId);
    void removeByTypeId(TypeId typeId);

private:

    std::unordered_map<TypeId, Reflect::ClassInstance> data;
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

} // namespace Core

#endif /* __GLOBAL_DATA_HPP__ */