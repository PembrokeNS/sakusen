#!/usr/bin/env python
from sakusen import *
from sakusencomms import *
from PyQt4 import QtCore
from settingsDialog import Ui_settingsDialog
def debug(x): pass
Socket_socketsInit()
class joiner(QtCore.QObject):
	"""A class to open new connections"""
	def join(self,address):
		global debug
		debug("called join")
		"""Join a sakusen server at a tcp-based address"""
		d=JoinMessageData("")
		m=Message(d)
		d.thisown=False #m now owns d
		debug("created message")
		debug("address is "+str(address))
		s=Socket_newConnectionToAddress(str(address))
		s.setNonBlocking(True)
		debug("opened connection")
		s.send(m)
		b=uint8(BUFFER_LEN)
		t=timeval(5,0) #5 seconds
		l=s.receiveTimeout(b,BUFFER_LEN,t)
		i=IArchive(b,l)
		r=Message(i)
		t=r.getType()
		debug("about to send signal from joiner")
		if(t==messageType_accept):
			d=r.getAcceptData()
			self.emit(QtCore.SIGNAL("newConnection(PyQt_PyObject,PyQt_PyObject)"),s,d.getId().toString())
			debug("signal sent")
		else:
			debug("Got unexpected message type, dying")
			raise Exception()