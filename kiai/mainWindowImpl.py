#!/usr/bin/env python
from PyKDE4 import kdeui
from PyQt4 import QtCore
from mainWindow import Ui_mainWindow
class mainWindow(kdeui.KXmlGuiWindow):
	def __init__(self, environment, parent=None):
		kdeui.KXmlGuiWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)
		self.setupGUI()
		self.connect(self.ui.entryline, QtCore.SIGNAL('returnPressed(const QString &) '),self.runcommand)
		self.env = environment
	def runcommand(self, t):
		self.ui.output.append(">"+t)
		self.ui.entryline.setText("")
		exec str(t) in self.env
	def write(self, t):
		self.ui.output.append(t.strip())


