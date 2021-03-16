#ifndef __TYPE_ID_HPP__
#define __TYPE_ID_HPP__

using TypeId = int;
const TypeId InvalidTypeId = 0;

namespace Core {

TypeId GetNextTypeId();

template<typename T>
class TypeIdHelper {
public:
    static TypeId value() {
        static const TypeId typeId = GetNextTypeId();
        return typeId;
    }
};

} // namespace Core

template<typename T>
constexpr TypeId GetTypeId() {
    return Core::TypeIdHelper<T>::value();
}

#endif /* __TYPE_ID_HPP__ */