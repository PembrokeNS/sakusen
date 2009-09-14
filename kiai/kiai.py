#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore, QtGui, QtOpenGL
from PyKDE4 import kdecore,kdeui
from connectDialog import Ui_connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from gameModel import *
from modeltest import ModelTest
from mainWindowImpl import mainWindow
from mapModel import mapModel
from sceneModel import sceneModel
import sys, imp, string
from sakusen_resources import fileUtils_getHome, path
from sakusen import CONFIG_SUBDIR
from sakusen_client import *
from sakusen_comms import *

def debug(x): pass

KIAI_SUBDIR="kiai"
interestingthings = {} #the things we want user scripting to be able to interact with

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
class connectDialog(QtGui.QDialog):
	def __init__(self,parent=None):
		QtGui.QDialog.__init__(self,parent)
		self.ui=Ui_connectDialog()
		self.ui.setupUi(self)
		self.connect(self,QtCore.SIGNAL("accepted()"),self.openConnection)
	def openConnection(self):
		global debug
		debug("called openConnection")
		self.emit(QtCore.SIGNAL("openConnection(QString)"),self.ui.address.text())
Socket_socketsInit()
def join(address):
	"""Join a sakusen server at a tcp-based address"""
	d=JoinMessageData("")
	m=Message(d)
	d.thisown=False #m now owns d
	s=Socket_newConnectionToAddress(str(address))
	s.setNonBlocking(True)
	s.send(m)
	b=uint8(BUFFER_LEN)
	t=timeval(5,0) #5 seconds
	l=s.receiveTimeout(b,BUFFER_LEN,t)
	i=IArchive(b,l)
	r=Message(i)
	t=r.getType()
	if(t==messageType_accept):
		d=r.getAcceptData()
		openSettingsDialog(s,d.getId().toString())
	else:
		debug("Got unexpected message type, dying")
                raise Exception()
class listener(QtCore.QObject):
	"""A class to handle sockets and emit signals when messages are received"""
	def __init__(self):
		QtCore.QObject.__init__(self)
		self.activeSocket=None
		self.clientid=None
		self.game=None
	def addSocket(self,s,clientid=0):
		debug("Adding a socket")
		self.activeSocket=s
		self.clientid=clientid
		self.game=gameModel(clientid)
		self.setSetting(('clients',str(clientid),'application','name'),'Kiai')
	def checkPendingSockets(self):
		b=uint8(BUFFER_LEN)
		if(self.activeSocket):
			s=self.activeSocket
			l=s.receive(b,BUFFER_LEN)
			if(l):
				debug("Received %d bytes from socket"%l)
				i=IArchive(b,l)
				if(sakusen_client.cvar.world):
					playerid=sakusen_client.cvar.world.getPlayerId()
				else:
					playerid=PlayerId.invalid()
				debug("Deserializing message")
				m=Message(i,playerid,self.game.resourceinterface)
				debug("Getting message type")
				t=m.getType()
				debug("Message is of type %d"%t)
				if(t==messageType_notifySetting):
					d=m.getNotifySettingData()
					if(d.getSetting()==":game:universe:name"):
						try:
							self.game.setUniverse(d.getValue()[0])
						except Exception:
							#should do some sort of error handling here
							print "Unable to find universe \"%s\""%d.getValue()[0]
					self.emit(QtCore.SIGNAL("settingsNotification(PyQt_PyObject)"),d)
				elif(t==messageType_gameStart):
					d=m.getGameStartData()
					debug("Server wants us to start a game, our id is %s, topology %s, corners %s and %s, gravity %s, heightfield %s"%(`d.getPlayerId()`,`d.getTopology()`,`d.getTopRight()`,`d.getBottomLeft()`,`d.getGravity()`,`d.getHeightfield()`))
					startGame(d,self.game)
					#hope that's acted on before we do this
					self.game.createWorld(d)
					QtCore.QObject.connect(self,QtCore.SIGNAL("update(PyQt_PyObject)"),self.game.pushUpdates)
				elif(t==messageType_update):
					d=m.getUpdateData()
					self.emit(QtCore.SIGNAL("update(PyQt_PyObject)"),d)
				else:
					debug("Received unexpected message of type %d"%m.getType())
	def requestSetting(self,path):
		s=string.join(path,':')
		d=GetSettingMessageData(s)
		m=Message(d)
		d.thisown=0 #m now owns d
		self.activeSocket.send(m)
	def leave(self):
		d=LeaveMessageData()
		m=Message(d)
		d.thisown=0 #m now owns d
		self.activeSocket.send(m)
	def setSetting(self,path,value):
		s=string.join(path,':')
		d=ChangeSettingMessageData(s,value)
		m=Message(d)
		d.thisown=0 #m now owns d
		self.activeSocket.send(m)
def openSettingsDialog(socket,clientid):
	global a,l,s,m,mainwindow #otherwise s and m will get hilariously garbage-collected
	l.addSocket(socket,clientid)
	QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),l.leave)
	s = settingsDialog()
	mainwindow.ui.dock.setWidget(s)
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
	mainwindow.ui.gameview.setRubberBandSelectionMode(QtCore.Qt.ContainsItemShape)
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
l=listener()
t=QtCore.QTimer()
QtCore.QObject.connect(w,QtCore.SIGNAL("accepted()"),w.openConnection)
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),join)
QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),l.checkPendingSockets)
t.start(10) #value in miliseconds - might want to make this less for the actual release, and more when debugging
w.show()
sys.stdout = mainwindow
sys.stderr = mainwindow
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)
