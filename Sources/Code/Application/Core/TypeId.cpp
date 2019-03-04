#include "Core/TypeId.hpp"

namespace Core
{

TypeId GLOBAL_TYPE_ID = InvalidTypeId;

TypeId GetNextTypeId() {
    ++GLOBAL_TYPE_ID;
    return GLOBAL_TYPE_ID;
}

} // namespace Core
