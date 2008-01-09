#!/usr/bin/env python
from PyQt4 import QtGui
from mainWindow import Ui_mainWindow
class mainWindow(QtGui.QMainWindow):
	def __init__(self,parent=None):
		QtGui.QMainWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)