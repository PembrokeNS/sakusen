#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore, QtGui, Qwt5

from orientationDialog import Ui_orientationDialog

class OrientationDialog(QtGui.QWidget, Ui_orientationDialog):
	def __init__(self, *args):
		QtGui.QWidget.__init__(self, *args)
		Ui_orientationDialog.__init__(self, *args)
		self.setupUi(self)
	#	self.setWindowFlags(QtCore.Qt.Popup)
		self.rose = Qwt5.QwtSimpleCompassRose()
		self.compass.setRose(self.rose)
		self.needle = Qwt5.QwtCompassMagnetNeedle()
		self.compass.setNeedle(self.needle)
		self.connect(self.ok, QtCore.SIGNAL("clicked()"), self.onClick)
	def onClick(self):
		self.emit(QtCore.SIGNAL("valueChanged(double)"), self.compass.value())
		self.close()
