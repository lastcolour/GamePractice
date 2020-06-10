from PyQt5.QtWidgets import QWidget, QLabel, QTreeWidget, QVBoxLayout, QTreeWidgetItem
from PyQt5.QtCore import Qt

from .EditVec3Value import EditVec3Value
from .EditQuatValue import EditQuatValue

from utils.ViewUtils import GetMinimunWidgetTreeHeight
from native.ValueNative import ValueType
from view.LogicView import LogicView

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

    def getVal(self):
        return 0, 0, 0, 0

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

    def setEditEntity(self, entity):
        if entity is None:
            self._setupLogic(EmptyTransformLogic())
            self._tree.setEnabled(False)
        else:
            self._setupLogic(entity.getTransformLogic())
            self._tree.setEnabled(False)
