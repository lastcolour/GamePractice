from PyQt6 import QtCore
from PyQt6.QtWidgets import QLabel, QLineEdit, QDialog, QTreeWidget, QVBoxLayout, QPushButton, QHBoxLayout, \
    QTreeWidgetItem, QStyle
from utils.Managers import GetEventManager
from utils.ViewUtils import FilterTreeBySearchText
from model.AssetsModel import FileNodeType
from native.ValueNative import ResourceType

from dialog.FilePreview import FilePreview

class UniversalSelectDialog(QDialog):
    def __init__(self):
        super().__init__()

        self._selectResult = None

        self._rootLayout = QVBoxLayout()

        self._infoLabel = QLabel()
        self._rootLayout.addWidget(self._infoLabel)

        self._lineEdit = QLineEdit()
        self._lineEdit.setPlaceholderText("Search...")
        self._lineEdit.setClearButtonEnabled(True)
        self._lineEdit.textChanged.connect(self._signal_search_textChanged)
        self._rootLayout.addWidget(self._lineEdit)

        self._treeLayout = QHBoxLayout()

        self._tree = QTreeWidget()
        self._tree.setHeaderHidden(True)
        self._tree.setColumnCount(1)
        self._tree.setSortingEnabled(False)
        self._tree.currentItemChanged.connect(self._signal_tree_currentItemChanged)
        self._treeLayout.addWidget(self._tree)

        self._preview = FilePreview()
        self._treeLayout.addWidget(self._preview)

        self._rootLayout.addLayout(self._treeLayout)

        self._buttotLayout = QHBoxLayout()

        self._cancelBt = QPushButton("Cancel")
        self._cancelBt.clicked.connect(self._signal_cancelBt_clicked)
        self._buttotLayout.addWidget(self._cancelBt)

        self._buttotLayout.addStretch()

        self._selectBt = QPushButton("Select")
        self._selectBt.clicked.connect(self._signal_selectBt_clicked)
        self._selectBt.setEnabled(False)
        self._buttotLayout.addWidget(self._selectBt)

        self._rootLayout.addLayout(self._buttotLayout)

        self.setMinimumSize(600, 600)
        self.setLayout(self._rootLayout)
        self.setWindowTitle("Universal Select Dialog")

    def reject(self):
        self._selectResult = None
        super().reject()

    def getSelectResult(self):
        return self._selectResult

    def _signal_cancelBt_clicked(self):
        self._selectResult = None
        self.done(0)

    def _signal_selectBt_clicked(self):
        self.done(0)

    def _signal_search_textChanged(self, text):
        FilterTreeBySearchText(self._tree, text)

    def _signal_tree_currentItemChanged(self, currItem, prevItem):
        if currItem is not None and hasattr(currItem, "_node"):
            self._selectResult = currItem._node
            self._selectBt.setEnabled(True)
            if not self._preview.isHidden():
                self._preview._setItemForPreview(currItem._node)
        else:
            self._selectResult = None
            self._selectBt.setEnabled(False)
            if not self._preview.isHidden():
                self._preview._setItemForPreview(None)

def _hideEmptyDirs(root):
    if hasattr(root, "_node") and not root._node.isDir():
        return False
    canHide = True
    for i in range(root.childCount()):
        if not _hideEmptyDirs(root.child(i)):
            canHide = False
    root.setHidden(canHide)
    return canHide

def _isFileTypeResourceTypeMatch(fileType, resourceType):
    if resourceType == ResourceType.SoundEvent:
        return True
    if fileType == FileNodeType.Entity:
        return resourceType == ResourceType.Entity
    elif fileType == FileNodeType.Image:
        return resourceType == ResourceType.Image
    elif fileType == FileNodeType.Sound:
        return resourceType == ResourceType.Sound
    else:
        return False

def _allPassFilter(val):
    return False

def ExecResourceSelectDialog(resourceType, infoText=None, filter=_allPassFilter):
    dialog = UniversalSelectDialog()
    if infoText:
        dialog._infoLabel.setText(infoText)
    else:
        dialog._infoLabel.setText("Select Resource File")
    dialog._tree.clear()
    if resourceType == ResourceType.Image or resourceType == ResourceType.Sound:
        addList = [(dialog._tree.invisibleRootItem(),
            GetEventManager().getAssetsModel().getResourcesTree()),]
    elif resourceType == ResourceType.SoundEvent:
        dialog._preview.setHidden(True)
        addList = [(dialog._tree.invisibleRootItem(),
            GetEventManager().getSoundEventsModel().getResourceTree()),]
    elif resourceType == ResourceType.Entity:
        dialog._preview.setHidden(True)
        addList = [(dialog._tree.invisibleRootItem(),
            GetEventManager().getAssetsModel().getEntitiesTree()),]
    else:
        raise RuntimeError("Unknown resource type: {0}".format(resourceType))
    while addList:
        currItem, currNode = addList.pop()
        if not currNode.isDir():
            if _isFileTypeResourceTypeMatch(currNode._type, resourceType):
                treeItem = QTreeWidgetItem(currItem)
                treeItem.setText(0, currNode.getBaseName())
                treeItem._node = currNode
                treeItem.setIcon(0, dialog.style().standardIcon(QStyle.StandardPixmap.SP_FileIcon))
                if filter(currNode):
                    treeItem.setDisabled(True)
        else:
            treeItem = QTreeWidgetItem(currItem)
            treeItem.setText(0, currNode.getBaseName())
            treeItem._node = currNode
            treeItem.setIcon(0, dialog.style().standardIcon(QStyle.StandardPixmap.SP_DirIcon))
            for childNode in currNode:
                addList.append((treeItem, childNode))
    _hideEmptyDirs(dialog._tree.invisibleRootItem())
    dialog._tree.sortItems(1, QtCore.Qt.SortOrder.DescendingOrder)
    dialog.exec()
    return dialog.getSelectResult()

def ExecAddEntitySelectDialog(entity):
    def _filterCanAddToEntity(node):
        path = node.getRelativePath()
        return not entity.canAddChild(path)
    return ExecResourceSelectDialog(ResourceType.Entity,
        infoText="Select Child Entity for: <b>{0}</b>".format(entity.getName()),
        filter=_filterCanAddToEntity)

def ExecSelectFromChildrenEntities(entity):
    dialog = UniversalSelectDialog()
    dialog._preview.setHidden(True)
    dialog._infoLabel.setText("Select Entity From Children")

    addList = [(dialog._tree.invisibleRootItem(), entity),]
    while addList:
        rootItem, currEnt = addList.pop()
        currItem = QTreeWidgetItem(rootItem)
        currItem._node = currEnt
        if currEnt == entity:
            currItem.setText(0, "{0} (this)".format(entity.getName()))
        else:
            if currEnt.isInternal():
                currItem.setText(0, currEnt.getName())
            else:
                currItem.setText(0, "{0} {1}".format(currEnt.getName(), currEnt.getNameSuffix()))
        for childEnt in currEnt.getChildren():
            addList.append((currItem, childEnt))

    dialog._tree.sortItems(1, QtCore.Qt.SortOrder.DescendingOrder)
    dialog.exec()
    return dialog.getSelectResult()

def ExecEntityLogicSelectDialog():
    dialog = UniversalSelectDialog()
    dialog._preview.setHidden(True)
    dialog._infoLabel.setText("Select Entity Logic")

    rootItem = dialog._tree.invisibleRootItem()
    logicsModel = GetEventManager().getEntityLogicsModel()
    for module in logicsModel.getLogics():
        moduleItem = QTreeWidgetItem(rootItem)
        moduleItem.setText(0, module.getName())
        moduleItem.setIcon(0, dialog.style().standardIcon(QStyle.StandardPixmap.SP_DirIcon))
        for logic in module.getLogics():
            logicItem = QTreeWidgetItem(moduleItem)
            logicItem.setText(0, logic.getName())
            logicItem.setIcon(0, dialog.style().standardIcon(QStyle.StandardPixmap.SP_FileIcon))
            logicItem._node = logic

    dialog._tree.sortItems(1, QtCore.Qt.SortOrder.DescendingOrder)
    dialog.exec()
    return dialog.getSelectResult()