#!/usr/bin/env python
import string
from PyQt4 import QtCore
from sakusen import *
from sakusencomms import *
def debug(x): pass
class listener(QtCore.QObject):
	"""A class to handle sockets and emit signals when messages are received"""
	def __init__(self):
		QtCore.QObject.__init__(self)
		self.activeSockets=[]
		self.clientids=[]
	def addSocket(self,s,clientid=0):
		debug("Adding a socket")
		self.activeSockets.append(s)
		self.clientids.append(clientid)
		self.setSetting(('clients',str(clientid),'application','name'),'Kiai')
	def checkPendingSockets(self):
		debug("Checking pending sockets")
		b=uint8(BUFFER_LEN)
		for s in self.activeSockets:
			debug("checking socket "+`s`)
			l=s.receive(b,BUFFER_LEN)
			debug("received %d bytes from socket"%l)
			if(l):
				i=IArchive(b,l)
				m=Message(i)
				t=m.getType()
				if(t==messageType_notifySetting):
					d=m.getNotifySettingData()
					self.emit(QtCore.SIGNAL("settingsNotification(PyQt_PyObject)"),d)
				else:
					print "Received unexpected message of type %d"%m.getType()
			debug("checked socket "+`s`)
	def requestSetting(self,path):
		s=string.join(path,':')
		d=GetSettingMessageData(s)
		m=Message(d)
		d.thisown=0 #m now owns d
		for socket in self.activeSockets:
			socket.send(m)
	def leave(self):
		d=LeaveMessageData()
		m=Message(d)
		d.thisown=0 #m now owns d
		for socket in self.activeSockets:
			socket.send(m)
	def setSetting(self,path,value):
		s=string.join(path,':')
		d=ChangeSettingMessageData(s,value)
		m=Message(d)
		d.thisown=0 #m now owns d
		for socket in self.activeSockets:
			socket.send(m)
