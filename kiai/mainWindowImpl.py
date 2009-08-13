#!/usr/bin/env python
from PyKDE4 import kdeui
from mainWindow import Ui_mainWindow
class mainWindow(kdeui.KMainWindow):
	def __init__(self,parent=None):
		kdeui.KMainWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)

