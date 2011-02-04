#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PyQt4 import QtCore, QtGui
from PyKDE4 import kdeui
import sip

from sakusen import *
from sakusen_client import *
from sakusen_comms import *

import resources
from util import color
from sceneModel import mpeFunction, orderSelectedUnits, forWeapons
from singleton import interestingthings

selectedUnits = set()

class constructor:
	def __init__(self, cname, mainwindow):
		self.button = kdeui.KPushButton(mainwindow.ui.construct)
		self.button.setCheckable(True)
		self.button.setObjectName(cname)
		self.button.setText(cname)
		self.button.refcount = 1
		mainwindow.ui.construct.layout().addWidget(self.button)
		self.button.show()
		self.cname = cname
		self.mainwindow = mainwindow
		@mpeFunction
		@orderSelectedUnits
		@forWeapons('c:'+cname)
		def construct(**kwargs):
			ctype = kwargs['universe'].getUnitTypePtr(cname)
			csz = ctype.getDynamicData().getSize()
			cpt = SPoint32(kwargs['targetPoint'].x,kwargs['targetPoint'].y,kwargs['targetPoint'].z + csz.z)
			orient = Orientation()
			f = Frame(cpt, orient)
			return TargetFrameOrderData(kwargs['weaponId'],f)
		def setConstruct(flag):
			if(flag):
				self.mainwindow.ui.gameview.scene().mpe = construct
				self.mainwindow.othercommands(self.button)
		QtCore.QObject.connect(self.button, QtCore.SIGNAL('toggled(bool)'), setConstruct)

class setter:
	def __init__(self, cname, mainwindow):
		self.button = kdeui.KPushButton(mainwindow.ui.setters)
		self.button.setObjectName(cname)
		self.button.setText(cname)
		self.button.refcount = 1
		mainwindow.ui.setters.layout().addWidget(self.button)
		self.button.show()
		self.cname = cname
		self.mainwindow = mainwindow
		self.flag = False
		@orderSelectedUnits
		@forWeapons('n:'+self.cname)
		def setcmd(**kwargs):
			#TODO: don't really want to be prompting individually for each unit.
			return TargetNumberOrderData(kwargs['weaponId'], QtGui.QInputDialog.getInt(self.mainwindow, "Input setting", "Please enter the number to set %s to" % self.cname)[0])
		def set_():
			setcmd(selectedUnits = self.mainwindow.ui.gameview.scene().selectedItems(), socket = self.mainwindow.ui.gameview.scene().sock)
		QtCore.QObject.connect(self.button, QtCore.SIGNAL('clicked()'), set_)

class unitShape(QtGui.QGraphicsPolygonItem):
	def __init__(self, unit, polygon, mainwindow, parent = None):
		QtGui.QGraphicsPolygonItem.__init__(self, polygon, parent)
		self.unit = unit
		self.mainwindow = mainwindow
	def itemChange(self, change, value):
		if(change == QtGui.QGraphicsItem.ItemSelectedChange):
			value and selectedUnits.add(self) or selectedUnits.remove(self)
			s = self.unit.getStatus()
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
						if(value):
							b.refcount += 1
						else:
							b.refcount -= 1
							if( not b.refcount):
								sip.delete(b)
					else:
						assert(value) #no button, so we must be getting selected
						b = constructor(cname, self.mainwindow)
				if(w.getClientHint()[:2] == 'n:'):
					cname = w.getClientHint()[2:]
					b = self.mainwindow.ui.setters.findChild(kdeui.KPushButton, cname)
					if(b):
						#this is a manual reference-counting scheme to ensure that
						#there are always buttons available to build something so long
						#as any of the selected units can build it.
						#button exists, so cahnge its reference count
						if(value):
							b.refcount += 1
						else:
							b.refcount -= 1
							if( not b.refcount):
								sip.delete(b)
					else:
						assert(value) #no button, so we must be getting selected
						b = setter(cname, self.mainwindow)
			return value
		else:
			return value

class eventUnitFactory(UnitFactory):
	def __init__(self, scene, mainwindow):
		UnitFactory.__init__(self)
		self.scene = scene
		self.mainwindow = mainwindow
		interestingthings['units'] = []
	def create(self,u):
		e=eventUpdatedUnit(u, self.scene, self.mapmodel, self.mainwindow)
		e = e.__disown__() #the C++ code will look after it.
		p=UpdatedUnit_CreateUpdatedUnitPtr(e)
		return p

class eventUpdatedUnit(UpdatedUnit):
	def __init__(self, u, scene, m, mainwindow):
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
		self.i.setPen(self.pen())
		self.icon = QtGui.QGraphicsPixmapItem(QtGui.QPixmap(':/pixmaps/sakusen-unit-ground01.PNG'), self.i)
		self.icon.setOffset((pos.x-self.scene.left)//self.scene.mapmodel.d, (pos.y-self.scene.bottom)//self.scene.mapmodel.d)
	def incrementState(self):
		flag = self.getAltered() or (not self.getStatus().velocity.isZero()) or (not self.getStatus().angularVelocity.isZero())
		UpdatedUnit.incrementState(self)
		if(flag): #have changed
			status = self.getStatus()
			frame = status.getFrame()
			pos = frame.getPosition()
			corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
			self.polygon = QtGui.QPolygonF()
			for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)//self.scene.mapmodel.d,(t.y-self.scene.bottom)//self.scene.mapmodel.d))
			self.i.setPolygon(self.polygon)
			self.i.setPen(self.pen())
			self.icon.setOffset((pos.x-self.scene.left)//self.scene.mapmodel.d, (pos.y-self.scene.bottom)//self.scene.mapmodel.d)
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
		interestingthings['units'].remove(self)

