def ClearLayout(layout):
    while layout.count() >= 1:
        item = layout.itemAt(layout.count() - 1)
        widget = item.widget()
        if widget is not None:
            layout.removeWidget(widget)
            widget.close()
            del widget

def _getWidgetTreeItemHeight(treeWidget, item):
    height = treeWidget.visualItemRect(item).height()
    for i in range(item.childCount()):
        childItem = item.child(i)
        childHeight = _getWidgetTreeItemHeight(treeWidget, childItem)
        height += childHeight
    return height

def GetMinimunWidgetTreeHeight(treeWidget):
    height = 0
    for i in range(treeWidget.topLevelItemCount()):
        height += 2
        item = treeWidget.topLevelItem(i)
        height += _getWidgetTreeItemHeight(treeWidget, item)
    if height > 0:
        height -= 1
    return height