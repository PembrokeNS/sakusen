#!/usr/bin/env python
import string
from PyQt4 import QtCore
from sakusen import *
from sakusencomms import *
from gameModel import gameModel
def debug(x): print x
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
		debug("Checking pending sockets")
		b=uint8(BUFFER_LEN)
		if(self.activeSocket):
			s=self.activeSocket
			debug("checking socket "+`s`)
			l=s.receive(b,BUFFER_LEN)
			debug("received %d bytes from socket"%l)
			if(l):
				i=IArchive(b,l)
				m=Message(i,self.game.playerid,self.game.resourceinterface)
				t=m.getType()
				if(t==messageType_notifySetting):
					d=m.getNotifySettingData()
					debug("comparing setting %s with %s"%(d.getSetting(),'clients:%s:player'%self.clientid))
					if(d.getSetting()==':clients:%s:player'%self.clientid):
						debug("changing player id to %s"%d.getValue())
						#self.game.playerid=PlayerId(d.getValue())
					self.emit(QtCore.SIGNAL("settingsNotification(PyQt_PyObject)"),d)
				#elif(t==messageType_gameStart):
				#	d=m.getGameStartData()
				#	print "Server wants us to start a game, our id is %d, topology %s, corners %s and %s, gravity %d, heightfield %s"%(d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
				else:
					print "Received unexpected message of type %d"%m.getType()
			debug("checked socket "+`s`)
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
