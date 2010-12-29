# -*- coding: utf-8 -*-

# Form implementation generated from reading ui file '../kiai/orientationDialog.ui'
#
# Created: Wed Dec 29 20:45:14 2010
#      by: PyQt4 UI code generator 4.8.2
#
# WARNING! All changes made in this file will be lost!

from PyQt4 import QtCore, QtGui

try:
    _fromUtf8 = QtCore.QString.fromUtf8
except AttributeError:
    _fromUtf8 = lambda s: s

class Ui_orientationDialog(object):
    def setupUi(self, orientationDialog):
        orientationDialog.setObjectName(_fromUtf8("orientationDialog"))
        orientationDialog.resize(114, 114)
        self.horizontalLayout = QtGui.QHBoxLayout(orientationDialog)
        self.horizontalLayout.setObjectName(_fromUtf8("horizontalLayout"))
        self.compass = Qwt5.QwtCompass(orientationDialog)
        self.compass.setLineWidth(4)
        self.compass.setObjectName(_fromUtf8("compass"))
        self.horizontalLayout.addWidget(self.compass)

        self.retranslateUi(orientationDialog)
        QtCore.QMetaObject.connectSlotsByName(orientationDialog)

    def retranslateUi(self, orientationDialog):
        orientationDialog.setWindowTitle(QtGui.QApplication.translate("orientationDialog", "Form", None, QtGui.QApplication.UnicodeUTF8))

from PyQt4 import Qwt5

class orientationDialog(QtGui.QWidget, Ui_orientationDialog):
    def __init__(self, parent=None, f=QtCore.Qt.WindowFlags()):
        QtGui.QWidget.__init__(self, parent, f)

        self.setupUi(self)

