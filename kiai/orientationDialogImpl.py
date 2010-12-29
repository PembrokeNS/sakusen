#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore

from orientationDialog import orientationDialog

class OrientationDialog(orientationDialog):
	def __init__(self, *args):
		orientationDialog.__init__(self, *args)
	#	self.setWindowFlags(QtCore.Qt.Popup)
