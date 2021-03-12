class MessageSystem:
    _INSTAMCE = None

    def __init__(self):
        MessageSystem._INSTAMCE = self
        self._registry = {}
        self._boundMethods = {}

    def sendMsg(self, msgObj):
        msgType = msgObj.__class__
        if msgType not in self._registry:
            return
        for callback in self._registry[msgType]:
            callback(msgObj)

    def registerForMsg(self, msgClass, receiver):
        if msgClass not in self._registry:
            self._registry[msgClass] = set()
        if receiver.__self__ not in self._boundMethods:
            self._boundMethods[receiver.__self__] = set()
        self._boundMethods[receiver.__self__].add(receiver)
        self._registry[msgClass].add(receiver)

    def unregisterFromMsg(self, msgClass, obj):
        if msgClass not in self._registry:
            return
        self._registry[msgClass] = set([x for x in self._registry[msgClass] if x.__self__ != obj])

    def unresgisterFromAllMsg(self, obj):
        for msgClass in self._registry:
            self.unregisterFromMsg(msgClass, obj)

def SendMessage(msgObj):
    MessageSystem._INSTAMCE.sendMsg(msgObj)

def RegisterForMessage(msgType, receiver):
    MessageSystem._INSTAMCE.registerForMsg(msgType, receiver)

def UnregisterFromMessage(msgType, obj):
    MessageSystem._INSTAMCE.unregisterFromMsg(msgType, obj)

def UnregisterFromAllMessages(obj):
    MessageSystem._INSTAMCE.unresgisterFromAllMsg(obj)