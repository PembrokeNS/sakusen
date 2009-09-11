#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
from sakusen import *
from sakusen_comms import *

class sceneModel(QtGui.QGraphicsScene):
	def __init__(self,mainwindow,socket):
		QtGui.QGraphicsScene.__init__(self)
		self.mw = mainwindow
		self.sock = socket
	def mousePressEvent(self,e):
		if(e.button() == QtCore.Qt.RightButton):
			p = e.lastScenePos()
			q = SPoint32(int(p.x()*100) + self.left,int(p.y()*100) + self.bottom,0)
			od = MoveOrderData(q)
			od.thisown = 0 # todo: rationalise ownership of this - we probably need to reconstruct it for each unit.
			o = Order(od)
			units = self.selectedItems()
			if(self.mw.ui.move.isChecked()):
				for i in units:
					u = i.unit
					om = OrderMessage(u.getId(), o)
					omd = OrderMessageData(om)
					m = Message(omd)
					omd.thisown = 0 #m now owns it
					self.sock.send(m)
		else:
			QtGui.QGraphicsScene.mousePressEvent(self,e)
