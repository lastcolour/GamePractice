from PyQt5.QtWidgets import QTreeWidgetItemIterator

import subprocess
import platform

def ClearLayout(layout):
    for i in reversed(range(layout.count())):
        widget = layout.itemAt(i).widget()
        if widget is not None:
            layout.removeWidget(widget)
            widget.setParent(None)
            widget.close()

def _getWidgetTreeItemHeight(treeWidget, item):
    height = treeWidget.visualItemRect(item).height()
    for i in range(item.childCount()):
        childItem = item.child(i)
        childHeight = _getWidgetTreeItemHeight(treeWidget, childItem)
        height += childHeight
    return height

def GetMinimunWidgetTreeHeight(treeWidget):
    height = 1
    for i in range(treeWidget.topLevelItemCount()):
        item = treeWidget.topLevelItem(i)
        height += _getWidgetTreeItemHeight(treeWidget, item) + 1
    return height

def OpenPlatformFileExplorer(filePath):
    if platform.system() == "Windows":
        filePath = filePath.replace("/", "\\")
        subprocess.Popen("explorer /select,\"{0}\"".format(filePath))
    elif platform.system() == "Darwin":
        subprocess.Popen(["open", filePath])
    else:
        subprocess.Popen(["xdg-open", filePath])

def _isSearchTextMatch(item, searchText):
    if searchText == "":
        return True
    else:
        itemText = item.text(0).lower()
        return searchText in itemText

def FilterTreeBySearchText(tree, text):
    searchText = text.lower()
    it = QTreeWidgetItemIterator(tree)
    while it.value():
        item = it.value()
        it += 1
        if not _isSearchTextMatch(item, searchText):
            item.setHidden(True)
        else:
            item.setHidden(False)
            parent = item.parent()
            while parent is not None:
                parent.setExpanded(True)
                parent.setHidden(False)
                parent = parent.parent()