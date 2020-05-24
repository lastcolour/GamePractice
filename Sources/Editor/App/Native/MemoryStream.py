import struct

class MemoryStream:
    def __init__(self, data):
        self._data = data
        self._pos = 0

    def reset(self):
        self._pos = 0

    def tellg(self):
        return self._pos

    def readBool(self):
        res = struct.unpack_from("<B", self._pos, self._data)
        self._pos += 1
        return res

    def readInt(self):
        res = struct.unpack_from("<B", self._pos, self._data)
        self._pos += 4
        return res

    def readFloat(self):
        res = struct.unpack_from("<B", self._pos, self._data)
        self._pos += 4
        return res

    def readUChar(self):
        res = struct.unpack_from("<B", self._pos, self._data)
        self._pos += 1
        return res

    def readStrign(self):
        res = struct.unpack_from("<B", self._pos, self._data)
        self._pos += 1
        return res

    def writeBool(self, val):
        pass

    def writeInt(self, val):
        pass

    def writeFloat(self, val):
        pass

    def writeUChar(self, val):
        pass

    def writeString(self, val):
        pass