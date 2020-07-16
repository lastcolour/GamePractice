#include "Entity/ETEntity.hpp"
#include "Core/ETPrimitives.hpp"

namespace EntityUtils {

const char* GetEntityName(EntityId entityId) {
    const char* name = "";
    ET_SendEventReturn(name, entityId, &ETEntity::ET_getName);
    return name;
}

} // namespace EntityUtils
