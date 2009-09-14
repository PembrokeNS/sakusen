#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyQt4 import QtCore, QtGui, QtOpenGL
from PyKDE4 import kdecore,kdeui
from connectDialogImpl import connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from gameModel import gameModel, eventUnitFactory, eventSensorReturnsFactory
from mainWindowImpl import mainWindow
from mapModel import mapModel
from sceneModel import sceneModel
import sys, imp, string
from sakusen_resources import fileUtils_getHome, path
from sakusen import *
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
Socket_socketsInit()
class socketModel():
	def __init__(self, address):
		self.s = Socket_newConnectionToAddress(address)
		self.s.setNonBlocking(True)
	def sendd(self, data):
		data.thisown = 0 #the Message will take care of it.
		self.s.send(Message(data))
	def receiveTimeout(self, buf, blen, tout):
		return self.s.receiveTimeout(buf, blen, tout)
	def receive(self, buf, blen):
		return self.s.receive(buf, blen)
def join(address):
	global interestingthings, activeSocket, game, t, clientid
	"""Join a sakusen server at a tcp-based address"""
	d=JoinMessageData("")
	s=socketModel(str(address))
	interestingthings['socket'] = s
	s.sendd(d)
	b=uint8(BUFFER_LEN)
	t=timeval(5,0) #5 seconds
	l=s.receiveTimeout(b,BUFFER_LEN,t)
	i=IArchive(b,l)
	r=Message(i)
	t=r.getType()
	if(t==messageType_accept):
		d=r.getAcceptData()
		activeSocket = s
		t=QtCore.QTimer()
		QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),checkPendingSockets)
		t.start(10) #value in miliseconds - might want to make this less for the actual release, and more when debugging
		clientid=d.getId().toString()
		game=gameModel(clientid)
		setSetting(('clients',str(clientid),'application','name'),'Kiai')
		openSettingsDialog(s,clientid)
	else:
		debug("Failed to connect to server!")
def checkPendingSockets():
	b=uint8(BUFFER_LEN)
	if(activeSocket):
		s=activeSocket
		l=s.receive(b,BUFFER_LEN)
		if(l):
			debug("Received %d bytes from socket"%l)
			i=IArchive(b,l)
			if(cvar.world):
				playerid=cvar.world.getPlayerId()
			else:
				playerid=PlayerId.invalid()
			debug("Deserializing message")
			me=Message(i,playerid,game.resourceinterface)
			debug("Getting message type")
			t=me.getType()
			debug("Message is of type %d"%t)
			if(t==messageType_notifySetting):
				d=me.getNotifySettingData()
				if(d.getSetting()==":game:universe:name"):
					try:
						game.setUniverse(d.getValue()[0])
					except Exception:
						#should do some sort of error handling here
						print "Unable to find universe \"%s\""%d.getValue()[0]
				m.processUpdate(d)
			elif(t==messageType_gameStart):
				d=me.getGameStartData()
				debug("Server wants us to start a game, our id is %s, topology %s, corners %s and %s, gravity %s, heightfield %s"%(`d.getPlayerId()`,`d.getTopology()`,`d.getTopRight()`,`d.getBottomLeft()`,`d.getGravity()`,`d.getHeightfield()`))
				startGame(d,game)
				game.createWorld(d)
			elif(t==messageType_update):
				d=me.getUpdateData()
				game.pushUpdates(d)
			else:
				debug("Received unexpected message of type %d"%me.getType())
def requestSetting(path):
	s=string.join(path,':')
	d=GetSettingMessageData(s)
	activeSocket.sendd(d)
def leave():
	try: #try and leave cleanly
		d=LeaveMessageData()
		activeSocket.sendd(d)
	except Exception: #but if we can't, don't worry
		pass
def setSetting(path,value):
	s=string.join(path,':')
	d=ChangeSettingMessageData(s,value)
	activeSocket.sendd(d)
def openSettingsDialog(socket,clientid):
	global a,l,m,mainwindow
	s = settingsDialog()
	mainwindow.ui.dock.setWidget(s)
	m=settingsModel(s)
	QtCore.QObject.connect(m,QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),requestSetting)
	QtCore.QObject.connect(m,QtCore.SIGNAL("editSetting(PyQt_PyObject,PyQt_PyObject)"),setSetting)
	s.ui.settingsTree.setModel(m)
	s.ui.settingsTree.setRootIndex(QtCore.QModelIndex())
	m.emit(QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),())
	s.show()
	userconfig("onconnect")
def startGame(d,g):
	global a,mainwindow,gamescene,mapmodel,l
	gamescene=sceneModel(mainwindow,activeSocket,g.universe)
	game.setScene(gamescene)
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
mainwindow = mainWindow(interestingthings)
w=connectDialog()
mainwindow.show()
QtCore.QObject.connect(w,QtCore.SIGNAL("accepted()"),w.openConnection)
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),join)
QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),leave)
w.show()
sys.stdout = mainwindow
sys.stderr = mainwindow
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)
