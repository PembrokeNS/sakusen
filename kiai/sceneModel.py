#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore,QtGui
from PyKDE4 import kdeui

from sakusen import *

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
			f(selectedUnits = self.selectedItems(), targetPoint = q, targetObject = self.itemAt(p), socket = self.sock)
	return g
