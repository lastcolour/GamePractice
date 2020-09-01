#ifndef __ET_THREAD_SAFE_HPP__
#define __ET_THREAD_SAFE_HPP__

#include <type_traits>

namespace ET {

struct ThreadSafeTrueFlag {
    int v;
};

struct ThreadSafeFalseFlag {};

template<typename T>
constexpr typename T::ThreadSafeFlag HasThreadSafeFlag(int v) {
    return typename T::ThreadSafeFlag{};
}

template<typename T>
constexpr bool HasThreadSafeFlag(bool v) {
    return false;
}

template<typename T>
constexpr bool IsThreadSafe() {
    if constexpr (std::is_same<decltype(HasThreadSafeFlag<T>(0)), ThreadSafeTrueFlag>::value) {
        return true;
    }
    return false;
}

} // namespace ETq

#endif /* __ET_THREAD_SAFE_HPP__ */