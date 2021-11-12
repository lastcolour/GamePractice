#ifndef __CLASS_INSTANCE_HPP__
#define __CLASS_INSTANCE_HPP__

namespace Reflect {

class ClassInfo;

class ClassInstance {

    using DeleteFuncT = void(*)(void*);

public:

    template<typename T>
    static ClassInstance CreateWithoutClassInfo(T* ptr) {
        static_assert(!std::is_same<T,void>::value, "Can't create instance of void*");
        auto deleteFunc = [](void* objectPtr){
            delete static_cast<T*>(objectPtr);
        };
        ClassInstance instance;
        instance.setDeleteFuncAndPtr(static_cast<DeleteFuncT>(deleteFunc), ptr);
        return instance;
    }

public:

    ClassInstance();
    ClassInstance(ClassInfo& clsInfo, void* clsInstance);
    ClassInstance(ClassInstance&& other);
    ClassInstance& operator=(ClassInstance&& other);
    ~ClassInstance();

    template<typename T>
    T* castTo() {
        if(!instance) {
            return nullptr;
        }
        auto typeId = Core::GetTypeId<T>();
        if(!isInstanceOfType(typeId)) {
            return nullptr;
        }
        return static_cast<T*>(instance);
    }

    void* get();
    const void* get() const;

    ClassInfo* getClassInfo();
    Core::TypeId getInstanceTypeId() const;
    bool readAllValuesFrom(const SerializeContext& ctx, const JSONNode& node);
    bool readAllValuesFrom(const SerializeContext& ctx, Memory::MemoryStream& stream);
    bool writeAllValuesTo(const SerializeContext& ctx, JSONNode& node);
    bool writeAllValuesTo(const SerializeContext& ctx, Memory::MemoryStream& stream);
    bool readValueFrom(const SerializeContext& ctx, ClassValueId valueId, const JSONNode& node);
    bool readValueFrom(const SerializeContext& ctx, ClassValueId valueId, Memory::MemoryStream& stream);
    bool writeValueTo(const SerializeContext& ctx, ClassValueId valueId, JSONNode& node);
    bool writeValueTo(const SerializeContext& ctx, ClassValueId valueId, Memory::MemoryStream& stream);
    bool addValueArrayElement(ClassValueId valueId);
    bool setValuePolymorphType(ClassValueId valueId, const char* typeName);

private:

    void setDeleteFuncAndPtr(DeleteFuncT deleteF, void* ptr);
    bool isInstanceOfType(Core::TypeId typeId) const;

private:

    ClassInstance(const ClassInstance&) = delete;
    ClassInstance& operator=(const ClassInstance&) = delete;

private:

    ClassInfo* classInfo;
    void* instance;
    DeleteFuncT deleteFunc;
};

} // namespace Reflect

#endif /* __CLASS_INSTANCE_HPP__ */