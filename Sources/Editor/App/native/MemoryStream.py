import struct

class MemoryStream:
    def __init__(self):
        self._data = bytearray()
        self._pos = 0

    def _grow(self, size):
        if len(self._data) > self._pos + size:
            return
        self._data = self._data.ljust(len(self._data) + max(size, 32))

    def getSize(self):
        return len(self._data)

    def setData(self, data):
        self._data = data
        self._pos = 0

    def getData(self):
        return self._data[:self._pos]

    def resetPos(self):
        self._pos = 0

    def tellg(self):
        return self._pos

    def readBool(self):
        res = struct.unpack_from("<?", self._data, self._pos)
        self._pos += 1
        return res[0]

    def readInt(self):
        res = struct.unpack_from("<i", self._data, self._pos)
        self._pos += 4
        return res[0]

    def readFloat(self):
        res = struct.unpack_from("<f", self._data, self._pos)
        self._pos += 4
        return res[0]

    def readUChar(self):
        res = struct.unpack_from("<B", self._data, self._pos)
        self._pos += 1
        return res[0]

    def readString(self):
        res = struct.unpack_from("<s", self._data, self._pos)
        self._pos += len(res)
        return res[0].decode('ascii')[:-1]

    def writeBool(self, val):
        self._grow(1)
        struct.pack_into("<?", self._data, self._pos, val)
        self._pos += 1

    def writeInt(self, val):
        self._grow(4)
        struct.pack_into("<i", self._data, self._pos, val)
        self._pos += 4

    def writeFloat(self, val):
        self._grow(4)
        struct.pack_into("<f", self._data, self._pos, val)
        self._pos += 4

    def writeUChar(self, val):
        self._grow(1)
        struct.pack_into("<B", self._data, self._pos, val)
        self._pos += 1

    def writeString(self, val):
        self._grow(len(val) + 1)
        struct.pack_into("<s", self._data, self._pos, val.encode('ascii'))
        self._pos += len(val)
        self._data[self._pos] = 0
        self._pos += 1