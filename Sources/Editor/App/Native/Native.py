class NativeObject:

    _NATIVE_API = None

    def __init__(self):
        pass

    def _getAPI(self):
        return NativeObject._NATIVE_API