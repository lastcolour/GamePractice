namespace EntityUtils {

const char* GetEntityName(EntityId entityId) {
    const char* name = "";
    ET_SendEventReturn(name, entityId, &ETEntity::ET_getName);
    return name;
}

std::vector<EntityLogic*> GetEntityLogicsByTypeId(Core::TypeId logicTypeId, EntityId entityId) {
    std::vector<EntityLogic*> logics;
    ET_SendEventReturn(logics, entityId, &ETEntity::ET_getLogisByTypeId, logicTypeId);
    return logics;
}

} // namespace EntityUtils
