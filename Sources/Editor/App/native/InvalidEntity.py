from .EntityNative import EntityNative

class InvalidEntity(EntityNative):
    def __init__(self, name):
        super().__init__()
        self._name = name

    def isInvalidEntity(self):
        return True

    def _syncWithNative(self):
        pass