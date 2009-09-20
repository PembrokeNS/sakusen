#!/usr/bin/env python

from PyQt4 import QtCore,QtGui
from PyKDE4 import kdeui

from sakusen import *
from sakusen_comms import *

class sceneModel(QtGui.QGraphicsScene):
	"""Encapsulates a model of the game world, and handles giving orders to it"""
	def __init__(self, mainwindow, socket, universe):
		# Needs access to mainwindow to get at buttons. Needs access to socket to send orders.
		# Needs access to universe to be able to grab unit type pointer stuff.
		QtGui.QGraphicsScene.__init__(self)
		self.mw = mainwindow
		self.sock = socket
		self.u = universe
	def mousePressEvent(self,e):
		if(e.button() == QtCore.Qt.RightButton):
			e.accept()
			p = e.lastScenePos()
			unitclicked = False
			srclicked = False
			if(self.itemAt(p)!=self.mapmodel.i): #clicked on a unit or sensor return
				try:
					clickedunit = self.itemAt(p).unit
					cu = clickedunit.getRefToThis()
					unitclicked = True
				except AttributeError: #not a unit
					sensorreturnclicked = True
					csr = self.itemAt(p).sr
					srclicked = True
			r = SPoint32(int(p.x()*self.mapmodel.d) + self.left,int(p.y()*self.mapmodel.d) + self.bottom,0)
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
					self.sock.sendd(omd)
			if(self.mw.ui.attack.isChecked()):
				 for i in units:
                                        u = i.unit
					s = u.getStatus()
					utype = s.getTypePtr()
					for j,w in enumerate(utype.getWeapons()):
						if(w.getClientHint() == 'o'):
							if(srclicked):
								od = TargetSensorReturnsOrderData(j, csr.getRefToThis())
							else:
								od = TargetPositionOrderData(j,q)
							od.thisown = 0 
							o = Order(od)
							om = OrderMessage(u.getId(), o)
							omd = OrderMessageData(om)
							self.sock.sendd(omd)
			for b in self.mw.ui.construct.findChildren(kdeui.KPushButton):
				if(b.isChecked()):
					for i in units:
						u = i.unit
                        	                s = u.getStatus()
                                	        utype = s.getTypePtr()
                                        	for j,w in enumerate(utype.getWeapons()):
                                        	        if(w.getClientHint()[:2] == 'c:'):
								cname = w.getClientHint()[2:]
								if(cname == b.objectName()):
									ctype = self.u.getUnitTypePtr(cname)
									csz = ctype.getDynamicData().getSize()
									cpt = SPoint32(q.x,q.y,q.z + csz.z)
									orient = Orientation()
									f = Frame(cpt, orient)
	                         					od = TargetFrameOrderData(j,f)
                                        				od.thisown = 0
                                                        		o = Order(od)
                                                        		om = OrderMessage(u.getId(), o)
                                                        		omd = OrderMessageData(om)
                                                        		self.sock.sendd(omd)
                        if(self.mw.ui.build.isChecked()):
                                for i in units:
                                        u = i.unit
                                        s = u.getStatus()
                                        utype = s.getTypePtr()
                                        for j,w in enumerate(utype.getWeapons()):
                                                if(w.getClientHint()[:2] == 'b'):
							if(unitclicked):
	                                                        od = TargetUnitOrderData(j,cu)
        	                                                od.thisown = 0
                	                                        o = Order(od)
                        	                                om = OrderMessage(u.getId(), o)
                                	                        omd = OrderMessageData(om)
                                                       		self.sock.sendd(omd)
