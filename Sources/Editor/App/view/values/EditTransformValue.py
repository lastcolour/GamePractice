from PyQt5.QtWidgets import QWidget, QLabel, QTreeWidget, QVBoxLayout, QTreeWidgetItem
from PyQt5.QtCore import Qt

from .EditVec3Value import EditVec3Value
from .EditQuatValue import EditQuatValue

from utils.ViewUtils import GetMinimunWidgetTreeHeight
from native.ValueNative import ValueType
from view.LogicView import LogicView

from msg.Messages import MsgSetEditEntity, MsgChangeEditEntity
from msg.MessageSystem import RegisterForMessage

class EmptyValue:
    def __init__(self, name):
        self._name = name

    def getName(self):
        return self._name

class EmptyVec3Value(EmptyValue):
    def __init__(self, name):
        super().__init__(name)

    def getType(self):
        return ValueType.Vec3

    def getVal(self):
        return 0, 0, 0

class EmptyQuatValue(EmptyValue):
    def __init__(self, name):
        super().__init__(name)

    def getType(self):
        return ValueType.Quat

    def setVal(self, x, y, z, w):
        pass

    def getVal(self):
        return 0, 0, 0, 1

class EmptyTransformLogic:
    def __init_(self):
        pass

    def getName(self):
        return "Transform"

    def getValues(self):
        return [EmptyVec3Value("pos"), EmptyVec3Value("scale"), EmptyQuatValue("rot")]

class EditTransformValue(LogicView):
    def __init__(self):
        super().__init__(EmptyTransformLogic())
        self._logicTopBar._removeLogicBt.hide()
        self._tree.setEnabled(False)
        self._setCanCopy(False)

        RegisterForMessage(MsgSetEditEntity, self._onSetEditEntity)
        RegisterForMessage(MsgChangeEditEntity, self._onSetEditEntity)

    def _onSetEditEntity(self, msg):
        entity = msg.entity
        if entity is None:
            self._setupLogic(EmptyTransformLogic())
            self._tree.setEnabled(False)
        else:
            self._setupLogic(entity.getTransformLogic())
            self._tree.setEnabled(msg.canEditTransform)
