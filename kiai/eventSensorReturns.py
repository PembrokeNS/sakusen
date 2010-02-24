#!/usr/bin/env python

from PyQt4 import QtCore, QtGui
import sip

from sakusen import *
from sakusen_client import *

from util import color

class eventSensorReturnsFactory(SensorReturnsFactory):
	def __init__(self,scene):
		SensorReturnsFactory.__init__(self)
		self.scene = scene
	def create(self,u):
		e=eventSensorReturns(u,self.scene,self.mapmodel)
		e = e.__disown__()
		p=UpdatedSensorReturns_CreateUpdatedSensorReturnsPtr(e)
		return p

class eventSensorReturns(UpdatedSensorReturns):
	def __init__(self,u,scene,m):
		UpdatedSensorReturns.__init__(self,u)
		self.scene = scene
		self.m = m
		self.i = None
		self.j = None
		self.altered()
	def altered(self):
		p = self.getPerception()
		if (p & sakusen.perception_unit):
			r = self.getUnit()
			self.u = sakusen.CompleteUnit(r) #the unit
			status = self.u.getStatus()
			frame = status.getFrame()
			pos = frame.getPosition()
			typedata = self.u.getTypeData()
			size = typedata.getSize()
			self.size = UPoint32(size) #make a copy of size, since we're going to let u get deleted eventually
			corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
			self.polygon = QtGui.QPolygonF()
			for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)//self.scene.mapmodel.d,(t.y-self.scene.bottom)//self.scene.mapmodel.d))
			if(not self.i):
				self.i=QtGui.QGraphicsPolygonItem(self.polygon,self.m.i)
				self.i.setPen(QtGui.QPen(QtGui.QColor('purple')))
				self.i.sr = self
			else:
				self.i.setPolygon(self.polygon)
			curhp = self.u.getStatus().getHitPoints().numify()
			tmaxhp = self.u.getStatus().getTypePtr().getDynamicData().getMaxHitPoints().numify()
			maxhp = self.u.getTypeData().getMaxHitPoints().numify()
			if(tmaxhp != maxhp):
				#still being built
				self.i.setBrush(QtGui.QBrush(QtGui.QColor('blue')))
	       		else:
				self.i.setBrush(QtGui.QBrush(color(curhp, maxhp)))
			if(self.j): sip.delete(self.j)
			self.j = None
		elif (p & sakusen.perception_owner):
			print("Able to perceive owner but not full unit. Unimplemented!!")
		elif (p & sakusen.perception_region):
			region = self.getRegion()
			rect = region.getBoundingRectangle()
			if(not self.j):
				self.j=QtGui.QGraphicsEllipseItem((rect.minx-self.scene.left + 0.0)//self.scene.mapmodel.d,(rect.miny-self.scene.bottom + 0.0)//self.scene.mapmodel.d,(rect.maxx-rect.minx + 0.0)//self.scene.mapmodel.d,(rect.maxy-rect.miny + 0.0)//self.scene.mapmodel.d,self.m.i)
				p = QtGui.QPen(QtCore.Qt.DashLine)
				p.setColor(QtGui.QColor('purple'))
				p.thisown = 0
				self.j.setPen(p)
				self.j.sr = self
			else:
				self.j.setRect((rect.minx-self.scene.left + 0.0)//self.scene.mapmodel.d,(rect.miny-self.scene.bottom + 0.0)//self.scene.mapmodel.d,(rect.maxx-rect.minx + 0.0)//self.scene.mapmodel.d,(rect.maxy-rect.miny + 0.0)//self.scene.mapmodel.d)
			if(self.i): sip.delete(self.i)
			self.i = None
		else:
			if(self.i): sip.delete(self.i)
			if(self.j): sip.delete(self.j)
			self.i = None
			self.j = None
	def destroying(self):
		if(self.i): sip.delete(self.i)
		if(self.j): sip.delete(self.j)
		self.i = None
		self.j = None

