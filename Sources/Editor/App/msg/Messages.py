def _canEditEntityTransform(entity):
    if entity is None:
        return False
    currParent = entity.getParent()
    while currParent != None:
        if not currParent.isInternal() and currParent.getParent() != None:
            return False
        currParent = currParent.getParent()
    return True

def _canEditEntityLogics(entity):
    currEntity = entity
    while currEntity != None:
        if not currEntity.isInternal() and currEntity.getParent() != None:
            return False
        currEntity = currEntity.getParent()
    return True

class MsgOpenEntityForEdit:
    def __init__(self, entityName):
        self.entityName = entityName

class MsgSetEditEntity:
    def __init__(self, entity):
        self.entity = entity
        self.canEditTransform = _canEditEntityTransform(entity)
        self.canEditLogics = _canEditEntityLogics(entity)
        if entity is not None:
            entity._syncWithNative()

class MsgChangeEditEntity(MsgSetEditEntity):
    def __init__(self, entity):
        super().__init__(entity)

class MsgOnAddLogicBtPressed:
    def __init__(self, entity):
        self.entity = entity

class MsgAddLogicToEntity:
    def __init__(self, entity, logic):
        self.entity = entity
        self.logic = logic

class MsgOnLogicDataEdited:
    def __init__(self, value):
        self.value = value