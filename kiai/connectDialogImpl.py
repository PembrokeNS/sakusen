#!/usr/bin/env python

from PyQt4 import QtCore
from PyKDE4 import kdeui

from connectDialog import Ui_connectDialog

class connectDialog(kdeui.KDialog):
	def __init__(self,parent=None):
		kdeui.KDialog.__init__(self,parent)
		self.ui=Ui_connectDialog()
		self.ui.setupUi(self)
		self.connect(self,QtCore.SIGNAL("accepted()"),self.openConnection)
	def openConnection(self):
		self.emit(QtCore.SIGNAL("openConnection(QString)"),self.ui.address.text())

