import subprocess
import platform

def ClearLayout(layout):
    while layout.count() >= 1:
        item = layout.itemAt(layout.count() - 1)
        widget = item.widget()
        if widget is not None:
            layout.removeWidget(widget)
            widget.close()
            del widget

def _getWidgetTreeItemHeight(treeWidget, item):
    height = treeWidget.visualItemRect(item).height() + 1
    for i in range(item.childCount()):
        childItem = item.child(i)
        childHeight = _getWidgetTreeItemHeight(treeWidget, childItem)
        height += childHeight + 1
    return height

def GetMinimunWidgetTreeHeight(treeWidget):
    height = 0
    for i in range(treeWidget.topLevelItemCount()):
        item = treeWidget.topLevelItem(i)
        height += _getWidgetTreeItemHeight(treeWidget, item)
    return height

def OpenPlatformFileExplorer(filePath):
    if platform.system() == "Windows":
        filePath = filePath.replace("/", "\\")
        subprocess.Popen("explorer /select,\"{0}\"".format(filePath))
    elif platform.system() == "Darwin":
        subprocess.Popen(["open", filePath])
    else:
        subprocess.Popen(["xdg-open", filePath])