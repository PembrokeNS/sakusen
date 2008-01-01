#!/usr/bin/env python
from PyQt4 import QtCore
from sakusen import *
from sakusencomms import *
def debug(x): pass
class listener(QtCore.QObject):
	"""A class to handle sockets and emit signals when messages are received"""
	def __init__(self):
		QtCore.QObject.__init__(self)
		self.activeSockets=[]
	def addSocket(self,s):
		self.activeSockets.append(s)
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
				print "Received unexpected message of type %d"%m.getType()
			debug("checked socket "+`s`)
