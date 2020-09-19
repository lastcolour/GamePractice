from PyQt5.QtWidgets import QWidget, QVBoxLayout, QHBoxLayout, QLabel, QPushButton, QTreeWidget, QTreeWidgetItem, QStyle, QFrame
from PyQt5.Qt import QSizePolicy
from PyQt5.QtCore import Qt

from native.ValueNative import ValueType
from utils.ViewUtils import GetMinimunWidgetTreeHeight
from utils.EventManager import GetEventManager

from menu.EntityLogicMenu import EntityLogicMenu

from .values.EditIntValue import EditIntValue
from .values.EditFloatValue import EditFloatValue
from .values.EditBoolValue import EditBoolValue
from .values.EditStringValue import EditStringValue
from .values.EditVec2iValue import EditVec2iValue
from .values.EditVec2Value import EditVec2Value
from .values.EditVec3Value import EditVec3Value
from .values.EditVec4Value import EditVec4Value
from .values.EditQuatValue import EditQuatValue
from .values.EditColorValue import EditColorValue
from .values.EditEnumValue import EditEnumValue
from .values.EditArrayValue import EditArrayValue
from .values.EditEntityValue import EditEntityValue
from .values.EditResourceValue import EditResourceValue

def _removeAllItemChildren(item):
    for i in reversed(range(item.childCount())):
        item.removeChild(item.child(i))

class LogicViewTopBar(QWidget):
    def __init__(self):
        super().__init__()

        self._canCopy = True

        self._entityLogic = None

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.StyledPanel)
        self._frame.setFrameShadow(QFrame.Plain)

        self._frameLayout = QHBoxLayout()
        self._frameLayout.setSpacing(1)
        self._frameLayout.setContentsMargins(0, 0, 0, 0)
        self._frame.setContextMenuPolicy(Qt.CustomContextMenu)
        self._frame.customContextMenuRequested.connect(self._signal_frame_contexMenuRequested)

        self._expandLogicBt = QPushButton()
        self._expandLogicBt.setIcon(self.style().standardIcon(QStyle.SP_TitleBarShadeButton))
        self._expandLogicBt.setFlat(True)
        self._frameLayout.addWidget(self._expandLogicBt)

        self._logicNameLabel = QLabel()
        self._frameLayout.addWidget(self._logicNameLabel)

        self._frameLayout.addStretch()

        self._removeLogicBt = QPushButton()
        self._removeLogicBt.setIcon(self.style().standardIcon(QStyle.SP_TitleBarCloseButton))
        self._removeLogicBt.setFlat(True)
        self._removeLogicBt.clicked.connect(self._signal_removeBt_clicked)
        self._frameLayout.addWidget(self._removeLogicBt)

        self._frame.setLayout(self._frameLayout)

        self._rootLayout = QVBoxLayout()
        self._rootLayout.setSpacing(0)
        self._rootLayout.setContentsMargins(0, 0, 0, 0)

        self._rootLayout.addWidget(self._frame)

        self.setLayout(self._rootLayout)

    def _setupLogic(self, entityLogic):
        self._entityLogic = entityLogic
        self._logicNameLabel.setText(self._entityLogic.getName())

    def _setCanCopy(self, copyFlag):
        self._canCopy = copyFlag

    def _signal_removeBt_clicked(self):
        GetEventManager().onRemoveEntityLogicBtClicked(self._entityLogic)

    def _signal_frame_contexMenuRequested(self, pt):
        if not self._canCopy:
            return
        menu = EntityLogicMenu(self)
        globalPt = self._frame.mapToGlobal(pt)
        menu.onMenuRequestedOnLogic(globalPt, self._entityLogic)

class LogicView(QWidget):
    def __init__(self, entityLogic):
        super().__init__()

        self._entityLogic = entityLogic

        self._frame = QFrame()
        self._frame.setFrameStyle(QFrame.WinPanel)
        self._frame.setFrameShadow(QFrame.Raised)
        self._frame.setLineWidth(3)

        self._frameLayout = QVBoxLayout()
        self._frameLayout.setContentsMargins(0, 0, 0, 0)
        self._frameLayout.setSpacing(0)

        self._logicTopBar = LogicViewTopBar()
        self._frameLayout.addWidget(self._logicTopBar)

        self._tree = QTreeWidget()
        self._tree.setColumnCount(2)
        self._tree.setHeaderHidden(True)
        self._tree.setSelectionMode(QTreeWidget.NoSelection)
        self._tree.verticalScrollBar().setEnabled(False)
        self._tree.setVerticalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self._tree.setHorizontalScrollBarPolicy(Qt.ScrollBarAlwaysOff)
        self._tree.itemCollapsed.connect(self._signal_tree_itemCollapseExpanded)
        self._tree.itemExpanded.connect(self._signal_tree_itemCollapseExpanded)
        self._tree.setIndentation(12)
        self._frameLayout.addWidget(self._tree)

        self._frame.setLayout(self._frameLayout)

        self._rootLayout = QVBoxLayout()
        self._rootLayout.setContentsMargins(0, 0, 0, 0)
        self._rootLayout.setSpacing(0)
        self._rootLayout.addWidget(self._frame)
        self.setLayout(self._rootLayout)

        self._setupLogic(self._entityLogic)

    def _setCanCopy(self, copyFlag):
        self._logicTopBar._setCanCopy(copyFlag)

    def _updateTreeSize(self):
        minTreeHeight = GetMinimunWidgetTreeHeight(self._tree)
        self._tree.setMaximumHeight(minTreeHeight)
        self._tree.setMinimumHeight(minTreeHeight)

        minHeight = minTreeHeight + self._logicTopBar.minimumSizeHint().height() + 4
        self.setMinimumHeight(minHeight)
        self.setMaximumHeight(minHeight)
        self.setMinimumWidth(self._tree.sizeHint().width())

    def _setupLogic(self, logic):
        self._entityLogic = logic
        self._logicTopBar._setupLogic(self._entityLogic)
        self._tree.clear()
        self._buildTree(self._tree, self._tree.invisibleRootItem(), self._entityLogic.getValues())
        self._updateTreeSize()

    def _buildArrayTree(self, widgetTree, parentItem, arrayEdit, values):
        _removeAllItemChildren(parentItem)
        for value in values:
            if value.getType() == ValueType.Object:
                item = QTreeWidgetItem(parentItem)
                widgetTree.setItemWidget(item, 0, QLabel("<b>{0}</b>".format(value.getName())))
                widgetTree.setItemWidget(item, 1, arrayEdit.createObjecValueWrap(value))
                self._buildTree(widgetTree, item, value.getValues())
                item.setExpanded(True)
            elif value.getType() == ValueType.Array:
                raise RuntimeError("Array of arrys not supported")
            else:
                item = QTreeWidgetItem(parentItem)
                item.setText(0, value.getName())
                editWidget = self._createEditWidget(value)
                widgetTree.setItemWidget(item, 1, arrayEdit.createSimpleWrap(editWidget))
        parentItem.setExpanded(True)

    def _buildTree(self, widgetTree, rootItem, values):
        for value in values:
            if value.getType() == ValueType.Object:
                item = QTreeWidgetItem(rootItem)
                widgetTree.setItemWidget(item, 0, QLabel("<b>{0}</b>".format(value.getName())))
                self._buildTree(widgetTree, item, value.getValues())
                item.setExpanded(True)
            elif value.getType() == ValueType.Array:
                item = QTreeWidgetItem(rootItem)
                widgetTree.setItemWidget(item, 0, QLabel("<b>{0}</b>".format(value.getName())))
                arrayEdit = self._createEditWidget(value)
                arrayEdit._rootArrayItem = item
                arrayEdit._logicView = self
                widgetTree.setItemWidget(item, 1, arrayEdit.createTitle())
                self._buildArrayTree(widgetTree, item, arrayEdit, value.getValues())
                item.setExpanded(True)
            else:
                item = QTreeWidgetItem(rootItem)
                item.setText(0, value.getName())
                widgetTree.setItemWidget(item, 1, self._createEditWidget(value))

    def _signal_tree_itemCollapseExpanded(self, item):
        self._updateTreeSize()

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
        elif valType == ValueType.Quat:
            return EditQuatValue(value)
        elif valType == ValueType.Color:
            return EditColorValue(value)
        elif valType == ValueType.Resource:
            return EditResourceValue(value)
        elif valType == ValueType.Entity:
            return EditEntityValue(value)
        elif valType == ValueType.Array:
            return EditArrayValue(value)
        elif valType == ValueType.Enum:
            return EditEnumValue(value)
        elif valType == ValueType.Object:
            raise RuntimeError("Object does not have edit widget")
        else:
            raise RuntimeError("Unknown Value Type '{0}'".format(valType))