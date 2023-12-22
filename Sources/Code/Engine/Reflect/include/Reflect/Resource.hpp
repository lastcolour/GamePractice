#ifndef __RESOURCE_HPP__
#define __RESOURCE_HPP__

namespace Reflect {

namespace Impl {

class BaseResource {
public:

    static const size_t MaxStorageTypeSize = 64u;

    struct DataAccessor {
        void* RuntimeData;
        void* isSetFlag;
    };

public:

    virtual ~BaseResource() = default;
    virtual DataAccessor getDataAccessor() = 0;
};

} // namepsace

template<typename ResourceSpec>
class Resource : public Impl::BaseResource {
public:

    using StorageType = typename ResourceSpec::StorageType;
    using RuntimeType = typename ResourceSpec::RuntimeType;
    using SpecType = ResourceSpec;

public:

    Resource() :
        data(),
        isSetFlag(false) {}

    Resource(const Resource<SpecType>& other) :
        data(other.data),
        isSetFlag(other.isSetFlag) {}

    Resource(Resource<SpecType>&& other) :
        data(std::move(other.data)),
        isSetFlag(other.isSetFlag) {
        
        other.isSetFlag = false;
    }

    Resource& operator=(const Resource<SpecType>& other) {
        data = other.data;
        isSetFlag = other.isSetFlag;
        return *this;
    }

    Resource& operator=(Resource<SpecType>&& other) {
        if(this != &other) {
            data = std::move(other.data);
            isSetFlag = other.isSetFlag;
            other.isSetFlag = false;
        }
        return *this;
    }

    virtual ~Resource() = default;

    bool isSet() const { return isSetFlag; }
    RuntimeType& get() { return data; }
    const RuntimeType& get() const { return data; }

    void set(const RuntimeType& newData) { data = newData; isSetFlag = true; }
    void set(RuntimeType&& newData) { data = std::move(newData); isSetFlag = true; }

    template<typename OtherType>
    void set(const OtherType& newData) {
        static_assert(std::is_convertible<OtherType, RuntimeType>::value, "Invalid type");
        data = newData;
        isSetFlag = true;
    }

    bool tryGet(RuntimeType& out) const {
        if(isSetFlag) {
            out = data;
        }
        return isSetFlag;
    }

    void setAndReset(const RuntimeType& newData) { data = newData; isSetFlag = false; }

    void reset() { data = RuntimeType(); isSetFlag = false; }

    virtual DataAccessor getDataAccessor() {
        return DataAccessor{&data, &isSetFlag};
    }

private:

    RuntimeType data;
    bool isSetFlag;
};

} // namespace Reflect

#endif /* __RESOURCE_HPP__ */