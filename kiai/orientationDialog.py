# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '../kiai/orientationDialog.ui'
#
# Created: Wed Dec 29 19:50:52 2010
#      by: PyQt4 UI code generator 4.8.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_OrientationDialog(object):
    def setupUi(self, OrientationDialog):
        OrientationDialog.setObjectName(_fromUtf8("OrientationDialog"))
        OrientationDialog.resize(114, 114)
        self.horizontalLayout = QtGui.QHBoxLayout(OrientationDialog)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.Compass = QwtCompass(OrientationDialog)
        self.Compass.setLineWidth(4)
        self.Compass.setObjectName(_fromUtf8("Compass"))
        self.horizontalLayout.addWidget(self.Compass)

        self.retranslateUi(OrientationDialog)
        QtCore.QMetaObject.connectSlotsByName(OrientationDialog)

    def retranslateUi(self, OrientationDialog):
        OrientationDialog.setWindowTitle(QtGui.QApplication.translate("OrientationDialog", "Form", None, QtGui.QApplication.UnicodeUTF8))

from qwt_compass import QwtCompass

class OrientationDialog(QtGui.QWidget, Ui_OrientationDialog):
    def __init__(self, parent=None, f=QtCore.Qt.WindowFlags()):
        QtGui.QWidget.__init__(self, parent, f)

        self.setupUi(self)

