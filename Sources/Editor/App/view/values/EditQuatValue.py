from PyQt5.QtWidgets import QWidget, QHBoxLayout, QLabel
from PyQt5.QtCore import Qt

from view.values.EditWidgets import EditFloatSpinBox

import math

from msg.Messages import MsgOnLogicDataEdited
from msg.MessageSystem import SendMessage

def _convertToEualerAngles(x, y, z, w):
    sinY = 2 * (w * z + x * y)
    cosY = 1 - 2 * (y * y + z * z)
    yaw = math.atan2(sinY, cosY)

    sinP = 2 * (w * y - z * x)
    if abs(sinP) >= 1:
        if sinP > 0:
            pitch = math.pi
        else:
            pitch = -math.pi
    else:
        pitch = math.asin(sinP)

    sinR = 2 * (w * x + y * z)
    cosR = 1 - 2 * (x * x + y * y)
    roll = math.atan2(sinR, cosR)

    return yaw, pitch, roll

def _convertFromEualerAngles(yaw, pitch, roll):
    halfY = yaw * 0.5
    halfP = pitch * 0.5
    halfR = roll * 0.5

    cy = math.cos(halfY)
    sy = math.sin(halfY)
    cp = math.cos(halfP)
    sp = math.sin(halfP)
    cr = math.cos(halfR)
    sr = math.sin(halfR)

    x = sr * cp * cy - cr * sp * sy
    y = cr * sp * cy + sr * cp * sy
    z = cr * cp * sy - sr * sp * cy
    w = cr * cp * cy + sr * sp * sy

    return x, y, z, w

class EditQuatValue(QWidget):
    def __init__(self, value):
        super().__init__()

        self._val = value

        self._rootLayout = QHBoxLayout()

        self._xLabel = QLabel("<b>X:</b>")
        self._rootLayout.addWidget(self._xLabel)

        self._xSpinBox = EditFloatSpinBox()
        self._xSpinBox.setSingleStep(1)
        self._xSpinBox.setMinimum(-180)
        self._xSpinBox.setMaximum(180)
        self._xSpinBox.valueChanged.connect(self._signal_xSpinBox_valueChanged)
        self._rootLayout.addWidget(self._xSpinBox)

        self._yLabel = QLabel("<b>Y:</b>")
        self._rootLayout.addWidget(self._yLabel)

        self._ySpinBox = EditFloatSpinBox()
        self._ySpinBox.setSingleStep(1)
        self._ySpinBox.setMinimum(-180)
        self._ySpinBox.setMaximum(180)
        self._ySpinBox.valueChanged.connect(self._signal_ySpinBox_valueChanged)
        self._rootLayout.addWidget(self._ySpinBox)

        self._zLabel = QLabel("<b>Z:</b>")
        self._rootLayout.addWidget(self._zLabel)

        self._zSpinBox = EditFloatSpinBox()
        self._zSpinBox.setSingleStep(1)
        self._zSpinBox.setMinimum(-180)
        self._zSpinBox.setMaximum(180)
        self._zSpinBox.valueChanged.connect(self._signal_zSpinBox_valueChanged)
        self._rootLayout.addWidget(self._zSpinBox)

        self._rootLayout.setContentsMargins(1, 1, 1, 1)
        self.setLayout(self._rootLayout)

        self._pull()

    def _signal_xSpinBox_valueChanged(self, newXVal):
        yAngle = self._ySpinBox.value()
        zAngle = self._zSpinBox.value()
        self._push(newXVal, yAngle, zAngle)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_ySpinBox_valueChanged(self, newYVal):
        xAngle = self._xSpinBox.value()
        zAngle = self._zSpinBox.value()
        self._push(xAngle, newYVal, zAngle)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _signal_zSpinBox_valueChanged(self, newZVal):
        xAngle = self._xSpinBox.value()
        yAngle = self._ySpinBox.value()
        self._push(xAngle, yAngle, newZVal)
        SendMessage(MsgOnLogicDataEdited(self._val))

    def _pull(self):
        x, y, z, w = self._val.getVal()
        yaw, pitch, roll = _convertToEualerAngles(x, y, z, w)

        yaw = math.degrees(yaw)
        pitch = math.degrees(pitch)
        roll = math.degrees(roll)

        currXAngle = self._xSpinBox.value()
        currYAngle = self._ySpinBox.value()
        currZAngle = self._zSpinBox.value()

        hasDiff = False
        if abs(currXAngle - pitch) > 0.001:
            hasDiff = True
        if abs(currYAngle - roll) > 0.001:
            hasDiff = True
        if abs(currZAngle - yaw) > 0.001:
            hasDiff = True
        if not hasDiff:
            return

        self._xSpinBox.setValue(pitch)
        self._ySpinBox.setValue(roll)
        self._zSpinBox.setValue(yaw)

    def _push(self, xAngle, yAngle, zAngle):
        xAngle = math.radians(xAngle)
        yAngle = math.radians(yAngle)
        zAngle = math.radians(zAngle)
        x, y, z, w = _convertFromEualerAngles(zAngle, xAngle, yAngle)
        self._val.setVal(x, y, z, w)