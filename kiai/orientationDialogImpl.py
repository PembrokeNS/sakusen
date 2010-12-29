#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore, Qwt5

from orientationDialog import orientationDialog

class OrientationDialog(orientationDialog):
	def __init__(self, *args):
		orientationDialog.__init__(self, *args)
	#	self.setWindowFlags(QtCore.Qt.Popup)
		self.rose = Qwt5.QwtSimpleCompassRose()
		self.compass.setRose(self.rose)
		self.needle = Qwt5.QwtCompassMagnetNeedle()
		self.compass.setNeedle(self.needle)
