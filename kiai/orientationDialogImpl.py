#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore

from orientationDialog import Ui_orientationDialog

class orientationDialog(Ui_orientationDialog):
	def __init__(self, *args):
		super.__init__(self, *args)
		self.setWindowFlags(QtCore.Qt.Popup)
