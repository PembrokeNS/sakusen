#!/usr/bin/env python
# -*- coding: utf-8 -*-

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
		self.sock = socket
		self.u = universe
	def mousePressEvent(self, e):
		self.mpe(self, e) # indirection, because dynamically assigning to this function doesn't work
	def mpe(self, *args): pass

def mpeFunction(f):
	def g(self,e):
		if(e.button() == QtCore.Qt.RightButton):
			e.accept()
			p = e.lastScenePos()
			r = SPoint32(int(p.x()*self.mapmodel.d) + self.left,int(p.y()*self.mapmodel.d) + self.bottom,0)
			q = SPoint32(r.x,r.y,self.mapmodel.h.getHeightAt(r))
			f(selectedUnits = self.selectedItems(), targetPoint = q, targetObject = self.itemAt(p), socket = self.sock, universe = self.u)
	return g

def orderSelectedUnits(f):
	def g(*args, **kwargs):
		for i in kwargs['selectedUnits']:
			kwargs['unit'] = i.unit
			od = f(*args, **kwargs)
			if(od):
				od.thisown = 0
				o = Order(od)
				om = OrderMessage(kwargs['unit'].getId(), o)
				omd = OrderMessageData(om)
				kwargs['socket'].sendd(omd)
	return g

def forWeapons(hint):
	def g(f, **kwargs):
		s = kwargs['unit'].getStatus()
		utype = s.getTypePtr()
		for j,w in enumerate(utype.getWeapons()):
			if(w.getClientHint().startswith(hint)):
				kwargs['weaponId'] = j
				return f(**kwargs)
	return lambda f: lambda **kwargs: g(f, **kwargs)
