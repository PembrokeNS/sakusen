from PyQt4 import QtGui, QtCore
from connectDialog import Ui_connectDialog

class connectDialog(QtGui.QDialog):
	def __init__(self,parent=None):
		QtGui.QDialog.__init__(self,parent)
		self.ui=Ui_connectDialog()
		self.ui.setupUi(self)
		self.connect(self,QtCore.SIGNAL("accepted()"),self.openConnection)
	def openConnection(self):
		self.emit(QtCore.SIGNAL("openConnection(QString)"),self.ui.address.text())

