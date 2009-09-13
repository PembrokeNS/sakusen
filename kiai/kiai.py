#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore,QtGui,QtOpenGL
from PyKDE4 import kdecore,kdeui
from connectDialog import Ui_connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from gameModel import *
from jointcp import joiner
from listen import listener
from modeltest import ModelTest
from mainWindowImpl import mainWindow
from mapModel import mapModel
from sceneModel import sceneModel
import sys,imp
from sakusen_resources import fileUtils_getHome,path
from sakusen import CONFIG_SUBDIR
from sakusen_client import *
def debug(x): print x

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
aboutdata = kdecore.KAboutData("kiai","",kdecore.ki18n("Kiai"),"0.0.4 気持ち",kdecore.ki18n("Sakusen client"),kdecore.KAboutData.License_Custom,kdecore.ki18n("(c) 2007-9 IEG/lmm"),kdecore.ki18n("none"),"none","md401@srcf.ucam.org") # necessary to keep a reference to this around, otherwise it gets GCed at the wrong time and we segfault. Call that a pykde bug.
kdecore.KCmdLineArgs.init(sys.argv, aboutdata)
a=kdeui.KApplication()
QtCore.pyqtRemoveInputHook() #to make debugging possible - remove in release version
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
	global a,l,s,m,mainwindow #otherwise s and m will get hilariously garbage-collected
	l.addSocket(socket,clientid)
	QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),l.leave)
	debug("going to open settings dialog") 
	s = settingsDialog()
	mainwindow.ui.dock.setWidget(s)
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
def startGame(d,g):
	global a,mainwindow,gamescene,mapmodel,l
	gamescene=sceneModel(mainwindow,l.activeSocket,g.universe)
	l.game.setScene(gamescene)
	#debug("Instantiating model of map "+`g.world.getMap()`)
	debug("Game started, creating world")
	e=eventUnitFactory(g.scene)
	e.__disown__() #w takes ownership of e; we have to do this differently because it is a director class, *sigh*
	sf=eventSensorReturnsFactory(g.scene)
	sf.__disown__() 
	g.w=PartialWorld(g.universe,e,sf,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
	#TODO: consistency between getMap and getPartialMap, depends on library organisation (sort of).
	gamescene.bottom=g.w.getMap().bottom()
	gamescene.left=g.w.getMap().left()
	mapmodel=mapModel(g.w.getPartialMap())
	gamescene.mapmodel = mapmodel
	sf.mapmodel = mapmodel
	e.mapmodel = mapmodel
	gamescene.addItem(mapmodel.i)
	mainwindow.ui.gameview.setViewport(QtOpenGL.QGLWidget())
	mainwindow.ui.gameview.setScene(gamescene)
	mainwindow.ui.gameview.centerOn(mapmodel.i) #for now, map should be initially centered
	mainwindow.ui.gameview.setResizeAnchor(QtGui.QGraphicsView.AnchorViewCenter)
	mainwindow.ui.gameview.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
	g.scene=gamescene
	debug("Scene is %s"%repr(g.scene))
	#QtCore.QObject.connect(l.game,QtCore.SIGNAL("unitCreated(PyQt_PyQbject,PyQt_PyQbject)"),gamescene.createUnit)
	#mainwindow.show()
mainwindow = mainWindow()
w=connectDialog()
#s = settingsDialog()
#d = QtGui.QDockWidget("Settings",mainwindow)
#d.setWidget(s)
mainwindow.show()
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
sys.stdout = mainwindow
sys.stderr = mainwindow
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)
