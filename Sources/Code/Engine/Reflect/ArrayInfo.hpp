#ifndef __ARRAY_INFO_HPP__
#define __ARRAY_INFO_HPP__

#include <vector>
#include <functional>

class ArrayInfo {
public:

    using CreateElemFuncT = std::function<void*(void*)>;

public:

    ArrayInfo();
    ~ArrayInfo();

    template<typename ElemT>
    void init() {
        using ArrayT = std::vector<ElemT>;
        createElemFunc = [](void* value) -> void* {
            auto arrayPtr = static_cast<ArrayT*>(valuePtr);
            arrayPtr->emplace_back();
            return static_cast<void*>(&(arrayPtr->back()));
        }
    }

    void* createElement(void* valuePtr);

private:

    CreateElemFuncT createElemFunc;
};

#endif /* __ARRAY_INFO_HPP__ */