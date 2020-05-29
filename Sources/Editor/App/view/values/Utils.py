def TextToFloat(text):
    if len(text) == 0:
        return 0.0
    return float(text)

def TextToInt(text):
    if len(text) == 0:
        return 0
    return int(text)

def SetFloatToLineEdit(lineEdit, val):
    currText = lineEdit.text()
    if currText == str(val):
        return
    cPos = lineEdit.cursorPosition()
    lineEdit.setText("{0}".format(val))
    lineEdit.setCursorPosition(cPos)

def SetIntToLineEdit(lineEdit, val):
    currText = lineEdit.text()
    if currText == str(val):
        return
    cPos = lineEdit.cursorPosition()
    lineEdit.setText("{0}".format(val))
    lineEdit.setCursorPosition(cPos)