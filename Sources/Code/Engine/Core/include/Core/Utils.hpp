#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <tuple>
#include <type_traits>

namespace Core {

template <typename ... ArgsT, typename FuncT, std::size_t... Idx>
void ApplyTuple(std::tuple<ArgsT ... >& tuple, FuncT&& func, std::index_sequence<Idx...>) {
    (void)std::initializer_list<int> {
        (func(std::get<Idx>(tuple)), void(), 0)...
    };
}

template <typename ... ArgsT, typename FuncT, std::size_t... Idx>
void ApplyTupleReverse(std::tuple<ArgsT ... >& tuple, FuncT&& func, std::index_sequence<Idx...>) {
    const auto tupleSize = std::tuple_size<std::tuple<ArgsT ... >>::value;
    (void)std::initializer_list<int> {
        (func(std::get<tupleSize - Idx - 1>(tuple)), void(), 0)...
    };
}

} // namespace Core

template <typename... ArgsT, typename FuncT>
void ApplyTuple(std::tuple<ArgsT ... >& tuple, FuncT&& func) {
    Core::ApplyTuple(tuple, func, std::index_sequence_for<ArgsT...>{});
}

template <typename... ArgsT, typename FuncT>
void ApplyTupleReverse(std::tuple<ArgsT ... >& tuple, FuncT&& func) {
    Core::ApplyTupleReverse(tuple, func, std::index_sequence_for<ArgsT...>{});
}

template <typename... ArgsT, typename FuncT, typename ResT>
void ApplyTupleResult(std::tuple<ArgsT ... >& tuple, FuncT&& func, std::vector<ResT>& results) {
    ApplyTuple(tuple, [&results, &func](auto&& elem){
        results.emplace_back(static_cast<ResT>(func(elem)));
    });
}

template<typename T>
bool EnumFlagsIntersect(T first, T second) {
    static_assert(std::is_enum<T>::value, "T isn't enum type");
    return static_cast<bool>(
        static_cast<int>(first) & static_cast<int>(second));
}

template<typename T>
T EnumFlagsUnite(T first, T second) {
    static_assert(std::is_enum<T>::value, "T isn't enum type");
    return static_cast<T>(
        static_cast<int>(first) | static_cast<int>(second));
}

#endif /* __UTILS_HPP__ */