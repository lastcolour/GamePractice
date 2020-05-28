from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton, QTreeWidget, QTreeWidgetItem, QStyle, QFrame
from PyQt5.Qt import QSizePolicy

from native.ValueNative import ValueType
from utils.ViewUtils import GetMinimunWidgetTreeHeight

from .values.EditIntValue import EditIntValue
from .values.EditFloatValue import EditFloatValue
from .values.EditBoolValue import EditBoolValue
from .values.EditStringValue import EditStringValue
from .values.EditVec2iValue import EditVec2iValue
from .values.EditVec2Value import EditVec2Value
from .values.EditVec3Value import EditVec3Value
from .values.EditVec4Value import EditVec4Value
from .values.EditColorValue import EditColorValue
from .values.EditEnumValue import EditEnumValue

class LogicViewTopBar(QWidget):
    def __init__(self, entityLogic):
        super().__init__()

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.WinPanel)
        self._frame.setFrameShadow(QFrame.Raised)
        self._frame.setLineWidth(2)
        self._frame.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Minimum)

        self._frameLayout = QHBoxLayout()

        self._expandLogicBt = QPushButton()
        self._expandLogicBt.setIcon(self.style().standardIcon(QStyle.SP_TitleBarShadeButton))
        self._expandLogicBt.setFlat(True)
        self._expandLogicBt.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        self._frameLayout.addWidget(self._expandLogicBt)

        self._logicNameLabel = QLabel(entityLogic.getName())
        self._frameLayout.addWidget(self._logicNameLabel)

        self._removeLogicBt = QPushButton()
        self._removeLogicBt.setIcon(self.style().standardIcon(QStyle.SP_TitleBarCloseButton))
        self._removeLogicBt.setFlat(True)
        self._removeLogicBt.setSizePolicy(QSizePolicy.Minimum, QSizePolicy.Minimum)
        self._frameLayout.addWidget(self._removeLogicBt)

        self._rootLayout = QVBoxLayout()
        self._rootLayout.addLayout(self._frameLayout)

        self.setLayout(self._rootLayout)

class LogicView(QWidget):
    def __init__(self, entityLogic):
        super().__init__()

        self._entityLogic = entityLogic

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.WinPanel)
        self._frame.setFrameShadow(QFrame.Raised)
        self._frame.setLineWidth(2)
        self._frame.setSizePolicy(QSizePolicy.Preferred, QSizePolicy.Minimum)

        self._frameLayout = QVBoxLayout()

        self._frameLayout.addWidget(LogicViewTopBar(entityLogic))

        self._tree = QTreeWidget()
        self._tree.setColumnCount(2)
        self._tree.setHeaderHidden(True)
        self._tree.setSelectionMode(QTreeWidget.NoSelection)
        self._buildTree(self._tree, self._tree.invisibleRootItem(), self._entityLogic.getValues())
        minTreeHeight = GetMinimunWidgetTreeHeight(self._tree)
        self._tree.setMaximumHeight(minTreeHeight)
        self._tree.setMinimumHeight(minTreeHeight)
        self._frameLayout.addWidget(self._tree)

        self._frameLayout.setContentsMargins(1, 1, 1, 1)
        self._frame.setLayout(self._frameLayout)

        self._rootLayout = QVBoxLayout()
        self._rootLayout.addWidget(self._frame)
        self.setLayout(self._rootLayout)
        self._rootLayout.setContentsMargins(1, 1, 1, 1)

    def _buildTree(self, widgetTree, rootItem, values):
        for value in values:
            if value.getType() == ValueType.Object:
                item = QTreeWidgetItem(rootItem)
                widgetTree.setItemWidget(item, 0, QLabel("<>{0}</>".format(value.getName())))
                self._buildTree(widgetTree, item, value.getValues())
                item.setExpanded(True)
            elif value.getType() == ValueType.Array:
                item = QTreeWidgetItem(rootItem)
                widgetTree.setItemWidget(item, 0, QLabel("<>{0}</>".format(value.getName())))
                self._buildTree(widgetTree, item, value.getValues())
                item.setExpanded(True)
            else:
                item = QTreeWidgetItem(rootItem)
                item.setText(0, value.getName())
                widgetTree.setItemWidget(item, 1, self._createEditWidget(value))

    def _createEditWidget(self, value):
        valType = value.getType()
        if valType == ValueType.Bool:
            return EditBoolValue(value)
        elif valType == ValueType.Int:
            return EditIntValue(value)
        elif valType == ValueType.Float:
            return EditFloatValue(value)
        elif valType == ValueType.String:
            return EditStringValue(value)
        elif valType == ValueType.Vec2i:
            return EditVec2iValue(value)
        elif valType == ValueType.Vec2:
            return EditVec2Value(value)
        elif valType == ValueType.Vec3:
            return EditVec3Value(value)
        elif valType == ValueType.Vec4:
            return EditVec4Value(value)
        elif valType == ValueType.Color:
            return EditIntValue(value)
        elif valType == ValueType.Resource:
            return EditStringValue(value)
        elif valType == ValueType.Entity:
            return EditStringValue(value)
        elif valType == ValueType.Array:
            return EditIntValue(value)
        elif valType == ValueType.Enum:
            return EditEnumValue(value)
        elif valType == ValueType.Object:
            raise RuntimeError("Object does not have edit widget")
        else:
            raise RuntimeError("Unknown Value Type '{0}'".format(valType))

