#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
from connectDialog import Ui_connectDialog
from settingsDialogImpl import settingsDialog
from jointcp import joiner
from listen import listener
import sys
def debug(x): print x
class connectDialog(QtGui.QDialog):
	def __init__(self,parent=None):
		QtGui.QDialog.__init__(self,parent)
		self.ui=Ui_connectDialog()
		self.ui.setupUi(self)
	def openConnection(self):
		global debug
		debug("called openConnection")
		self.emit(QtCore.SIGNAL("openConnection(QString)"),self.ui.address.text())
a=QtGui.QApplication(sys.argv)
a.setQuitOnLastWindowClosed(False) #may want to change this when main window is implemented
assert(not a.quitOnLastWindowClosed())
w=connectDialog()
s=settingsDialog()
j=joiner()
l=listener()
t=QtCore.QTimer()
QtCore.QObject.connect(w,QtCore.SIGNAL("accepted()"),w.openConnection)
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),j.join)
QtCore.QObject.connect(j,QtCore.SIGNAL("newConnection(PyQt_PyObject)"),l.addSocket)
QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),l.checkPendingSockets)
t.start(1000) #1 seconds - will want to make this less for the actual release
w.show()
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)