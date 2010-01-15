#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyKDE4 import kdeui
import sip

from sakusen import *
from sakusen_client import *

from util import color

class unitShape(QtGui.QGraphicsPolygonItem):
	def __init__(self, unit, polygon, mainwindow, parent = None):
		QtGui.QGraphicsPolygonItem.__init__(self, polygon, parent)
		self.unit = unit
		self.mainwindow = mainwindow
	def itemChange(self, change, value):
		if(change == QtGui.QGraphicsItem.ItemSelectedChange):
			s = self.unit.getStatus()
			d = value.toBool()
			utype = s.getTypePtr()
			for j,w in enumerate(utype.getWeapons()):
				if(w.getClientHint()[:2] == 'c:'):
					cname = w.getClientHint()[2:]
					b = self.mainwindow.ui.construct.findChild(kdeui.KPushButton, cname)
					if(b):
						#this is a manual reference-counting scheme to ensure that
						#there are always buttons available to build something so long
						#as any of the selected units can build it.
						#button exists, so cahnge its reference count
						if(d):
							b.refcount += 1
						else:
							b.refcount -= 1
							if( not b.refcount):
								sip.delete(b)
					else:
						assert(d) #no button, so we must be getting selected
						b = kdeui.KPushButton(self.mainwindow.ui.construct)
						b.setCheckable(True)
						b.setObjectName(cname)
						b.setText(cname)
						b.refcount = 1
						self.mainwindow.ui.construct.layout().addWidget(b)
						b.show()
			return value
		else:
			return value

class eventUnitFactory(UnitFactory):
	def __init__(self, interestingthings, scene, mainwindow):
		UnitFactory.__init__(self)
		self.interestingthings = interestingthings
		self.scene = scene
		self.mainwindow = mainwindow
		self.interestingthings['units'] = []
	def create(self,u):
		e=eventUpdatedUnit(self.interestingthings, u, self.scene, self.mapmodel, self.mainwindow)
		e = e.__disown__() #the C++ code will look after it.
		p=UpdatedUnit_CreateUpdatedUnitPtr(e)
		return p

class eventUpdatedUnit(UpdatedUnit):
	def __init__(self, interestingthings, u, scene, m, mainwindow):
		UpdatedUnit.__init__(self,u)
		self.scene = scene
		status = self.getStatus()
		frame = status.getFrame()
		pos = frame.getPosition()
		typedata = self.getTypeData()
		size = typedata.getSize()
		self.size = UPoint32(size) #make a copy of size, since we're going to let u get deleted eventually
		corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
		self.polygon = QtGui.QPolygonF()
		for t in corners: self.polygon.append(QtCore.QPointF((t.x-scene.left)//self.scene.mapmodel.d,(t.y-scene.bottom)//self.scene.mapmodel.d))
		self.i=unitShape(self, self.polygon, mainwindow, m.i)
		self.i.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
		interestingthings['units'].append(self)
		self.interestingthings = interestingthings
		self.i.setPen(self.pen())
	def incrementState(self):
		flag = self.getAltered() or (not self.getStatus().velocity.isZero()) or (not self.getStatus().angularVelocity.isZero())
		UpdatedUnit.incrementState(self)
		if(flag): #have changed
			status = self.getStatus()
			frame = status.getFrame()
			corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
			self.polygon = QtGui.QPolygonF()
			for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)//self.scene.mapmodel.d,(t.y-self.scene.bottom)//self.scene.mapmodel.d))
			self.i.setPolygon(self.polygon)
			self.i.setPen(self.pen())
	def pen(self): #create a pen of the appropriate colour for the current hitpoints and stuff
		curhp = self.getStatus().getHitPoints().numify()
		tmaxhp = self.getStatus().getTypePtr().getDynamicData().getMaxHitPoints().numify()
		maxhp = self.getTypeData().getMaxHitPoints().numify()
		if(tmaxhp != maxhp):
			#still being built
			return QtGui.QPen(QtGui.QColor('blue'))
		else:
			return QtGui.QPen(color(curhp, maxhp))
	def destroying(self):
		if(self.i.isSelected()):
			for w in self.getStatus().getTypePtr().getWeapons():
				if(w.getClientHint()[:2]=='c:'):
					b = self.i.mainwindow.ui.construct.findChild(kdeui.KPushButton, w.getClientHint()[2:])
					assert(b)
					b.refcount -= 1
					if( not b.refcount):
						sip.delete(b)
		sip.delete(self.i)
		self.interestingthings['units'].remove(self)

