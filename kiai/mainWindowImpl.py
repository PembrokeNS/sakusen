#!/usr/bin/env python
from PyKDE4 import kdeui
from PyQt4 import QtCore
from mainWindow import Ui_mainWindow
def debug(x): pass
class mainWindow(kdeui.KXmlGuiWindow):
	def __init__(self,parent=None):
		kdeui.KXmlGuiWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)
		self.setupGUI()
		self.connect(self.ui.entryline, QtCore.SIGNAL('returnPressed(const QString &) '),self.runcommand)
	def runcommand(self, t):
		debug("Called to run command: %s"%t)
		self.ui.output.append(">"+t)
		self.ui.entryline.setText("")
		exec(str(t))
	def write(self, t):
		self.ui.output.append(t.strip())


