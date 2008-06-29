#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
from connectDialog import Ui_connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from jointcp import joiner
from listen import listener
from modeltest import ModelTest
from mainWindowImpl import mainWindow
from mapModel import mapModel
from sceneModel import sceneModel
import sys,imp
from sakusen_resources import fileUtils_getHome,path
from sakusen import CONFIG_SUBDIR
def debug(x): pass

KIAI_SUBDIR="kiai"

def userconfig(s):
	configpath=fileUtils_getHome()
	configpath/=path(CONFIG_SUBDIR)
	configpath/=path(KIAI_SUBDIR)
	configpath/=path(s+".py")
	try:
		execfile(configpath.string())
	except IOError:
		print "No configuration file \"%s\" found, or failed to open that file"%(configpath.string())


userconfig("startup")
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
def openSettingsDialog(socket,clientid):
	global a,l,s,m #otherwise s and m will get hilariously garbage-collected
	l.addSocket(socket,clientid)
	QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),l.leave)
	debug("going to open settings dialog") 
	s=settingsDialog()
	QtCore.QObject.connect(s,QtCore.SIGNAL("rejected()"),a,QtCore.SLOT("quit()")) #this may need to be reconsidered in the final version
	m=settingsModel(s)
	QtCore.QObject.connect(m,QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),l.requestSetting)
	QtCore.QObject.connect(l,QtCore.SIGNAL("settingsNotification(PyQt_PyObject)"),m.processUpdate)
	QtCore.QObject.connect(m,QtCore.SIGNAL("editSetting(PyQt_PyObject,PyQt_PyObject)"),l.setSetting)
	QtCore.QObject.connect(l,QtCore.SIGNAL("gameStart(PyQt_PyObject,PyQt_PyObject)"),startGame)
	#mt=ModelTest(s,m)
	s.ui.settingsTree.setModel(m)
	s.ui.settingsTree.setRootIndex(QtCore.QModelIndex())
	m.emit(QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),())
	s.show()
	userconfig("onconnect")
	debug("opened a settings dialog")
def connectionfailed():
	print "Failed to connect to server - check the server is running"
	a.quit()
def startGame(d,w):
	global a,mainwindow,gamescene,mapmodel,l
	#TODO: disconnect s.rejected() and a.quit()
#	mapmodel = mapModel(w.getMap())
	gamescene=sceneModel()
	l.game.setScene(gamescene)
	debug("Instantiating model of map "+`w.getMap()`)
	mapmodel=mapModel(w.getMap())
	gamescene.s.addItem(mapmodel.i)
	mainwindow=mainWindow()
	mainwindow.ui.gameview.setScene(gamescene.s)
	QtCore.QObject.connect(l.game,QtCore.SIGNAL("unitCreated(PyQt_PyQbject,PyQt_PyQbject)"),gamescene.createUnit)
	mainwindow.show()
	a.setQuitOnLastWindowClosed(True) #game started, so next time we have no windows it'll be because we want to quit
a.setQuitOnLastWindowClosed(False)
assert(not a.quitOnLastWindowClosed())
w=connectDialog()
#s=settingsDialog()
j=joiner()
l=listener()
t=QtCore.QTimer()
QtCore.QObject.connect(w,QtCore.SIGNAL("accepted()"),w.openConnection)
QtCore.QObject.connect(w,QtCore.SIGNAL("rejected()"),a,QtCore.SLOT("quit()"))
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),j.join)
QtCore.QObject.connect(j,QtCore.SIGNAL("failure()"),connectionfailed)
#QtCore.QObject.connect(j,QtCore.SIGNAL("newConnection(PyQt_PyObject)"),l.addSocket) #needs to be synchronised with settingsDialog
QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),l.checkPendingSockets)
QtCore.QObject.connect(j,QtCore.SIGNAL("newConnection(PyQt_PyObject,PyQt_PyObject)"),openSettingsDialog)
t.start(10) #value in miliseconds - might want to make this less for the actual release, and more when debugging
w.show()
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)
