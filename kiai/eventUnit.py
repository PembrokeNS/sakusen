#!/usr/bin/env python

from PyQt4 import QtCore, QtGui

from sakusen import *
from sakusen_client import *

class eventUnitFactory(UnitFactory):
	def __init__(self,scene):
		UnitFactory.__init__(self)
		self.scene = scene
	def create(self,u):
		e=eventUpdatedUnit(u,self.scene,self.mapmodel)
		e = e.__disown__() #the C++ code will look after it.
		p=UpdatedUnit_CreateUpdatedUnitPtr(e)
		return p

class eventUpdatedUnit(UpdatedUnit):
	def __init__(self,u,scene,m):
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
                for t in corners: self.polygon.append(QtCore.QPointF((t.x-scene.left)/100,(t.y-scene.bottom)/100))
                self.i=QtGui.QGraphicsPolygonItem(self.polygon,m.i)
		self.i.setFlag(QtGui.QGraphicsItem.ItemIsSelectable)
		self.i.unit = self #we need to be able to get back to the unit from the item in the view.
	def incrementState(self):
		flag = self.getAltered() or (not self.getStatus().velocity.isZero()) or (not self.getStatus().angularVelocity.isZero())
		UpdatedUnit.incrementState(self)
		if(flag):
	                status = self.getStatus()
        	        frame = status.getFrame()
        	        corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
			self.polygon = QtGui.QPolygonF()
                	for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)/100,(t.y-self.scene.bottom)/100))
                	self.i.setPolygon(self.polygon)
	def destroying(self):
		sip.delete(self.i)

