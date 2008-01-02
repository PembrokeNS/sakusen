#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
from connectDialog import Ui_connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from jointcp import joiner
from listen import listener
from modeltest import ModelTest
import sys
def debug(x): pass
a=QtGui.QApplication(sys.argv)
class connectDialog(QtGui.QDialog):
	def __init__(self,parent=None):
		QtGui.QDialog.__init__(self,parent)
		self.ui=Ui_connectDialog()
		self.ui.setupUi(self)
	def openConnection(self):
		global debug
		debug("called openConnection")
		self.emit(QtCore.SIGNAL("openConnection(QString)"),self.ui.address.text())
def openSettingsDialog(socket):
	global a,l,s,m #otherwise s and m will get hilariously garbage-collected
	l.addSocket(socket)
	debug("going to open settings dialog") 
	s=settingsDialog()
	QtCore.QObject.connect(s,QtCore.SIGNAL("rejected()"),a,QtCore.SLOT("quit()")) #this may need to be reconsidered in the final version
	m=settingsModel(s)
	QtCore.QObject.connect(m,QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),l.requestSetting)
	QtCore.QObject.connect(l,QtCore.SIGNAL("settingsNotification(PyQt_PyObject)"),m.processUpdate)
	#mt=ModelTest(s,m)
	s.ui.settingsTree.setModel(m)
	s.ui.settingsTree.setRootIndex(QtCore.QModelIndex())
	m.emit(QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),())
	s.show()
	debug("opened a settings dialog")
a.setQuitOnLastWindowClosed(False) #may want to change this when main window is implemented
assert(not a.quitOnLastWindowClosed())
w=connectDialog()
#s=settingsDialog()
j=joiner()
l=listener()
t=QtCore.QTimer()
QtCore.QObject.connect(w,QtCore.SIGNAL("accepted()"),w.openConnection)
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),j.join)
#QtCore.QObject.connect(j,QtCore.SIGNAL("newConnection(PyQt_PyObject)"),l.addSocket) #needs to be synchronised with settingsDialog
QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),l.checkPendingSockets)
QtCore.QObject.connect(j,QtCore.SIGNAL("newConnection(PyQt_PyObject)"),openSettingsDialog)
t.start(1000) #1 seconds - will want to make this less for the actual release
w.show()
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)