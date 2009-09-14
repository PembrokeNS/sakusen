#!/usr/bin/env python

from settingsDialog import Ui_settingsDialog
from PyQt4 import QtGui

class settingsDialog(QtGui.QWidget):
	def __init__(self, model, root, parent = None):
		QtGui.QDialog.__init__(self,parent)
		self.ui=Ui_settingsDialog()
		self.ui.setupUi(self)
		self.ui.settingsTree.setModel(model)
		self.ui.settingsTree.setRootIndex(root)
