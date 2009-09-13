#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
from sakusen import *
from sakusen_comms import *

def debug(x): pass

class sceneModel(QtGui.QGraphicsScene):
	def __init__(self,mainwindow,socket, universe):
		QtGui.QGraphicsScene.__init__(self)
		self.mw = mainwindow
		self.sock = socket
		self.u = universe
	def mousePressEvent(self,e):
		if(e.button() == QtCore.Qt.RightButton):
			p = e.lastScenePos()
			r = SPoint32(int(p.x()*100) + self.left,int(p.y()*100) + self.bottom,0)
			q = SPoint32(r.x,r.y,self.mapmodel.h.getHeightAt(r))
			units = self.selectedItems()
			#TODO: be more intelligent about the following - they differ by very small amounts.
			if(self.mw.ui.move.isChecked()):
				for i in units:
					u = i.unit
					od = MoveOrderData(q)
					od.thisown = 0
					o = Order(od)
					om = OrderMessage(u.getId(), o)
					omd = OrderMessageData(om)
					m = Message(omd)
					omd.thisown = 0 #m now owns it
					self.sock.send(m)
			if(self.mw.ui.attack.isChecked()):
				 for i in units:
                                        u = i.unit
					s = u.getStatus()
					utype = s.getTypePtr()
					debug("Attacking at (%d,%d,%d)"%(q.x,q.y,q.z))
					for j,w in enumerate(utype.getWeapons()):
						debug("Considering weapon %d with hint %s"%(j,w.getClientHint()))
						if(w.getClientHint() == 'o'):
							od = TargetPositionOrderData(j,q)
							od.thisown = 0 
							o = Order(od)
							om = OrderMessage(u.getId(), o)
							omd = OrderMessageData(om)
							m = Message(omd)
							omd.thisown = 0 #m now owns it
							self.sock.send(m)
			if(self.mw.ui.create.isChecked()):
				for i in units:
					u = i.unit
                                        s = u.getStatus()
                                        utype = s.getTypePtr()
                                        for j,w in enumerate(utype.getWeapons()):
                                                if(w.getClientHint()[:2] == 'c:'):
							cname = w.getClientHint()[2:]
							ctype = self.u.getUnitTypePtr(cname)
							csz = ctype.getDynamicData().getSize()
							cpt = SPoint32(q.x,q.y,q.z + csz.z)
							orient = Orientation()
							f = Frame(cpt, orient)
							debug("Building at (%d,%d,%d)"%(cpt.x,cpt.y,cpt.z))
                                                        od = TargetFrameOrderData(j,f)
                                                        od.thisown = 0
                                                        o = Order(od)
                                                        om = OrderMessage(u.getId(), o)
                                                        omd = OrderMessageData(om)
                                                        m = Message(omd)
                                                        omd.thisown = 0 #m now owns it
                                                        self.sock.send(m)
                        if(self.mw.ui.build.isChecked()):
                                for i in units:
                                        u = i.unit
                                        s = u.getStatus()
                                        utype = s.getTypePtr()
                                        for j,w in enumerate(utype.getWeapons()):
                                                if(w.getClientHint()[:2] == 'b'):
                                                        od = TargetPositionOrderData(j,q)
                                                        od.thisown = 0
                                                        o = Order(od)
                                                        om = OrderMessage(u.getId(), o)
                                                        omd = OrderMessageData(om)
                                                        m = Message(omd)
                                                        omd.thisown = 0 #m now owns it
                                                        self.sock.send(m)

		else:
			QtGui.QGraphicsScene.mousePressEvent(self,e)
