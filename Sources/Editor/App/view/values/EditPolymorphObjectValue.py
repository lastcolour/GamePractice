from PyQt6.QtWidgets import QWidget, QHBoxLayout

from view.values.EditWidgets import EditComboBox

class PolymorphTypeSelector(EditComboBox):
    def __init__(self, value, treeItem, logicView):
        super().__init__()

        self._val = value
        self._rootTreeItem = treeItem
        self._logicView = logicView

        for item in value.getAllDerivedTypes():
            self.addItem(item)

        self._pull()

        self.currentIndexChanged.connect(self._signal_indexChanged)

    def _signal_indexChanged(self, idx):
        currentType = self.currentText()
        self._val.updatePolymorhType(currentType)
        self._logicView._buildTree(self._logicView._tree, self._rootTreeItem, self._val.getValues())
        self._logicView._updateTreeSize()

    def _pull(self):
        currType = self._val.getPolymorphType()
        idx = self.findText(currType)
        if idx == -1:
            raise RuntimeError("Can't find value '{0}' in combo box".format(currType))
        if idx != self.currentIndex():
            self.setCurrentIndex(idx)