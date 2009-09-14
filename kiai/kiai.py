#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui, QtOpenGL
from PyKDE4 import kdecore,kdeui
from connectDialogImpl import connectDialog
from settingsDialogImpl import settingsDialog
from settingsModel import settingsModel
from eventUnit import  eventUnitFactory
from eventSensorReturns import  eventSensorReturnsFactory
from mainWindowImpl import mainWindow
from mapModel import mapModel
from sceneModel import sceneModel

import sys

from sakusen_resources import *
from sakusen import *
from sakusen_client import *
from sakusen_comms import *

def debug(x): pass

KIAI_SUBDIR="kiai"
interestingthings = {} #the things we want user scripting to be able to interact with

def userconfig(s):
	"""Try and execute the specified user configuration file"""
	configpath=fileUtils_getHome()
	configpath/=path(CONFIG_SUBDIR)
	configpath/=path(KIAI_SUBDIR)
	configpath/=path(s+".py")
	try:
		execfile(configpath.string(),interestingthings)
	except IOError:
		print "No configuration file \"%s\" found, or failed to open that file"%(configpath.string())

class socketModel():
	"""Encapsulates a sakusen socket"""
	def join(self, address):
		global interestingthings, activeSocket, t, clientid, l, m
		"""Creates a socket, and connects it to the given address"""
		self.s = Socket_newConnectionToAddress(str(address))
		self.s.setNonBlocking(True)
		self.b = uint8(BUFFER_LEN) #try reusing our buffer
		self.game = False #whether there's a game in progress
		interestingthings['socket'] = activeSocket
		self.sendd(JoinMessageData(""))
		r = self.receivem(timeval(5, 0))
		if(r and r.getType() ==messageType_accept):
			d=r.getAcceptData()
			t=QtCore.QTimer()
			QtCore.QObject.connect(t,QtCore.SIGNAL("timeout()"),activeSocket.processm)
			t.start(10) #value in miliseconds - might want to make this less for the actual release, and more when debugging
			clientid=d.getId().toString()
			m = settingsModel(activeSocket)
			s = settingsDialog(m, QtCore.QModelIndex())
			mainwindow.ui.dock.setWidget(s)
			m.requestSetting(())
			s.show()
			m.setSetting(('clients',str(clientid),'application','name'),'Kiai')
			interestingthings['setSetting'] = m.setSetting #want to let users set settings
			userconfig("onconnect")
		else:
			print("Failed to connect to server!")
	def sendd(self, data):
		"""Sends a MessageData object"""
		data.thisown = 0 #the Message will take care of it.
		self.s.send(Message(data))
	def receivem(self, timeout = timeval(10,0)):
		"""Receives a Message and returns it"""
		l = self.s.receiveTimeout(self.b, BUFFER_LEN, timeout)
		if(l):
			i=IArchive(self.b,l)
			return Message(i)
		else:
			return None
	def processm(self):
		"""Receives a Message and processes it appropriately"""
		global a,mainwindow
		l=self.s.receive(self.b,BUFFER_LEN)
		if(l):
			i=IArchive(self.b,l)
			if(self.game):
				if(cvar.world):
					playerid=cvar.world.getPlayerId()
				else:
					#can this ever occur?
					playerid=PlayerId.invalid()
				me=Message(i,playerid,resourceinterface)
			else:
				me = Message(i)
			t=me.getType()
			if(t==messageType_notifySetting):
				d=me.getNotifySettingData()
				if(d.getSetting()==":game:universe:name"):
					#TODO: check hash
					result=resourceinterface.searchUniverse(d.getValue()[0])
					if(g(result,1)!=resourceSearchResult_success):
						print "Failed finding universe \"%s\", error %d"%(d.getValue()[0],g(result,1))
					else:
						self.universe = g(result,0)
				m.processUpdate(d)
			elif(t==messageType_gameStart):
				d=me.getGameStartData()
				gamescene=sceneModel(mainwindow,activeSocket,self.universe)
				debug("Game started, creating world")
				e=eventUnitFactory(gamescene)
				sf=eventSensorReturnsFactory(gamescene)
				self.pw = PartialWorld(self.universe,e,sf,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
				e = e.__disown__() #pw takes ownership of e and sf
				sf = sf.__disown__() 
				#TODO: consistency between getMap and getPartialMap, depends on library organisation (sort of).
				gamescene.bottom=self.pw.getMap().bottom()
				gamescene.left=self.pw.getMap().left()
				mapmodel=mapModel(self.pw.getPartialMap())
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
				self.game = True
			elif(t==messageType_update):
				d=me.getUpdateData()
				while(d.getTime()>cvar.world.getTimeNow()):
					cvar.world.endTick()
				if(d.getTime()>cvar.world.getTimeNow()):
					print "Got updates in wrong order"
				l=d.getUpdates()
				for u in l:
					cvar.world.applyUpdate(u)
			else:
				print("Received unexpected message of type %d"%me.getType())
	def leave(self):
		try: #try and leave cleanly
			d=LeaveMessageData()
			self.sendd(d)
		except Exception: #but if we can't, don't worry
			pass

userconfig("startup")
aboutdata = kdecore.KAboutData("kiai","",kdecore.ki18n("Kiai"),"0.0.4 気持ち",kdecore.ki18n("Sakusen client"),kdecore.KAboutData.License_Custom,kdecore.ki18n("(c) 2007-9 IEG/lmm"),kdecore.ki18n("none"),"none","md401@srcf.ucam.org") # necessary to keep a reference to this around, otherwise it gets GCed at the wrong time and we segfault. Call that a pykde bug.
kdecore.KCmdLineArgs.init(sys.argv, aboutdata)
a=kdeui.KApplication()
Socket_socketsInit()

d=fileUtils_getHome()
d/=path(CONFIG_SUBDIR)
d/=path(DATA_SUBDIR)
resourceinterface=FileResourceInterface_create(d,False)

mainwindow = mainWindow(interestingthings)
w=connectDialog()
mainwindow.show()
activeSocket = socketModel()
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),activeSocket.join)
QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),activeSocket.leave)
w.show()
sys.stdout = mainwindow
sys.stderr = mainwindow
r=a.exec_()
debug("Event loop terminated with status %d, dying"%r)
