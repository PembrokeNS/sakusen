#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui, QtOpenGL

from sakusen import *
from sakusen_comms import *
from sakusen_resources import *
from sakusen_client import *

from settingsModel import settingsModel
from eventUnit import  eventUnitFactory
from eventSensorReturns import  eventSensorReturnsFactory
from sceneModel import sceneModel
from mapModel import mapModel
from singleton import interestingthings

class socketModel():
	"""Encapsulates a sakusen socket"""
	def __init__(self, mainwindow, resourceinterface, userconfig):
		self.mainwindow = mainwindow
		self.resourceinterface = resourceinterface
		self.userconfig = userconfig
	def join(self, address):
		"""Creates a socket, and connects it to the given address"""
		self.s = Socket_newConnectionToAddress(str(address))
		self.s.setNonBlocking(True)
		self.b = uint8(SAKUSEN_COMMS_BUFFER_LEN) #try reusing our buffer
		self.game = False #whether there's a game in progress
		self.sendd(JoinMessageData(""))
		l = self.s.receiveTimeout(self.b, SAKUSEN_COMMS_BUFFER_LEN, timeval(5, 0))
		if(l):
			i=IArchive(self.b,l)
			r = Message(i)
			if(r.getType() ==messageType_accept):
				d=r.getAcceptData()
				self.t=QtCore.QTimer()
				QtCore.QObject.connect(self.t,QtCore.SIGNAL("timeout()"),self.processm)
				self.t.start(10) #value in miliseconds - might want to make this less for the actual release, and more when debugging
				clientid=d.getId().toString()
				self.m = settingsModel(self)
				self.mainwindow.ui.settingsTree.setModel(self.m)
				self.mainwindow.ui.settingsTree.setRootIndex(QtCore.QModelIndex())
				self.m.requestSetting(())
				self.m.requestSetting(('game','universe','name')) #so that we find the universe, if another player has already set it
				self.m.setSetting(('clients',str(clientid),'application','name'),'Kiai')
				self.m.setSetting(('clients',str(clientid),'application','version'),'"0.0.4 気持ち')
				interestingthings['setSetting'] = self.m.setSetting #want to let users set settings
				self.userconfig("onconnect")
			else:
				print("Server refused our connection")
		else:
			print("Failed to connect to server!")
	def sendd(self, data):
		"""Sends a MessageData object"""
		data.thisown = 0 #the Message will take care of it.
		self.s.send(Message(data))
	def processm(self):
		"""Receives a Message and processes it appropriately"""
		l=self.s.receive(self.b,SAKUSEN_COMMS_BUFFER_LEN)
		if(l):
			i=IArchive(self.b,l)
			if(self.game):
				if(cvar.world):
					playerid=cvar.world.getPlayerId()
				else:
					#can this ever occur?
					playerid=PlayerId.invalid()
				me=Message(i,playerid,self.resourceinterface)
			else:
				me = Message(i)
			t=me.getType()
			if(t==messageType_notifySetting):
				d=me.getNotifySettingData()
				if(d.getSetting()==":game:universe:name"):
					#TODO: check hash
					result=self.resourceinterface.searchUniverse(d.getValue()[0])
					if(g(result,1)!=resourceSearchResult_success):
						print("Failed finding universe \"%s\", error %d"%(d.getValue()[0],g(result,1)))
					else:
						self.universe = g(result,0)
				self.m.processUpdate(d)
			elif(t==messageType_gameStart):
				d=me.getGameStartData()
				gamescene=sceneModel(self.mainwindow,self,self.universe)
				e=eventUnitFactory(gamescene, self.mainwindow)
				sf=eventSensorReturnsFactory(gamescene)
				self.pw = PartialWorld(self.universe,e,sf,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
				e = e.__disown__() #pw takes ownership of e and sf
				sf = sf.__disown__() 
				#TODO: consistency between getMap and getPartialMap, depends on library organisation (sort of).
				gamescene.bottom=self.pw.getMap().bottom()
				gamescene.left=self.pw.getMap().left()
				mapmodel=mapModel(self.pw.getPartialMap(), d.getDexPerPixel())
				gamescene.mapmodel = mapmodel
				sf.mapmodel = mapmodel
				e.mapmodel = mapmodel
				gamescene.addItem(mapmodel.i)
				#self.mainwindow.ui.gameview.setViewport(QtOpenGL.QGLWidget())
				self.mainwindow.ui.gameview.setScene(gamescene)
				self.mainwindow.ui.gameview.centerOn(mapmodel.i) #for now, map should be initially centered
				self.mainwindow.ui.gameview.setResizeAnchor(QtGui.QGraphicsView.AnchorViewCenter)
				self.mainwindow.ui.gameview.setDragMode(QtGui.QGraphicsView.RubberBandDrag)
				self.mainwindow.ui.gameview.setRubberBandSelectionMode(QtCore.Qt.ContainsItemShape)
				self.game = True
				import order #FIXME: this is stupid
			elif(t==messageType_update):
				d=me.getUpdateData()
				while(d.getTime()>cvar.world.getTimeNow()):
					cvar.world.endTick()
				if(d.getTime()>cvar.world.getTimeNow()):
					print("Got updates in wrong order")
				l=d.getUpdates()
				for u in l:
					cvar.world.applyUpdate(u)
			elif(t==messageType_reject):
				d=me.getRejectData()
				print("Server refused our settings change: %s" %d.getReason())
			else:
				print(("Received unexpected message of type %d"%me.getType()))
			self.processm()
	def leave(self):
		try: #try and leave cleanly
			d=LeaveMessageData()
			self.sendd(d)
		except Exception: #but if we can't, don't worry
			pass
