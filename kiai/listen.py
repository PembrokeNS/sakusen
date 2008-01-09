#!/usr/bin/env python
import string
from PyQt4 import QtCore
from sakusen import *
from sakusencomms import *
import sakusenclient
from gameModel import gameModel
def debug(x): pass
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
		QtCore.QObject.connect(self,QtCore.SIGNAL("gameStart(PyQt_PyObject)"),self.game.createWorld)
	def checkPendingSockets(self):
		b=uint8(BUFFER_LEN)
		if(self.activeSocket):
			s=self.activeSocket
			l=s.receive(b,BUFFER_LEN)
			if(l):
				debug("Received %d bytes from socket"%l)
				i=IArchive(b,l)
				if(sakusenclient.cvar.world):
					playerid=sakusenclient.cvar.world.getPlayerId()
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
					self.emit(QtCore.SIGNAL("gameStart(PyQt_PyObject)"),d)
				else:
					print "Received unexpected message of type %d"%m.getType()
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
