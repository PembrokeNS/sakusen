#!/usr/bin/env python
# -*- coding: utf-8 -*-
#from __future__ import division #messes up other stuff
from sakusen import *
from sakusen_resources import *
from sakusen_client import *
import sakusen_client
from PyQt4 import QtCore,QtGui
import imp

def debug(x): pass

class eventUnitFactory(UnitFactory):
	def __init__(self,scene,interface=None): #TODO: remove default for second argument once we're actually using it
		UnitFactory.__init__(self)
		self.scene = scene
		self.interface=interface
	def create(self,u):
		e=eventUpdatedUnit(u,self.scene,self.mapmodel)
		debug("Creating eventUpdatedUnit "+`e`)
		e.__disown__() #the C++ code will look after it.
		p=UpdatedUnit_CreateUpdatedUnitPtr(e)
		debug("Returning pointer "+`p`)
		p.thisown=0 #I don't even know any more - TODO: CHECK THIS
		return p
class eventUpdatedUnit(QtCore.QObject,UpdatedUnit): #needs to inherit from UpdatedUnit *last*, otherwise swig directors fail. No idea whether signals will fail this way around, though.
	def __init__(self,u,scene,m):
		debug("Creating eventUpdatedUnit")
		UpdatedUnit.__init__(self,u)
		QtCore.QObject.__init__(self)
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
		debug("Created eventUpdatedUnit")
	def incrementState(self):
		debug("Incrementing state")
		flag = self.getAltered() or (not self.getStatus().velocity.isZero()) or (not self.getStatus().angularVelocity.isZero())
		debug(flag and "Need to update position" or "Don't need to update position")
		UpdatedUnit.incrementState(self)
		debug("Ran library state increment")
		if(flag):
	                status = self.getStatus()
        	        frame = status.getFrame()
        	        corners = [frame.localToGlobal(sakusen.SPoint32(x,y,z)) for (x,y,z) in ((self.size.x,self.size.y,self.size.z),(-self.size.x,self.size.y,self.size.z),(-self.size.x,-self.size.y,self.size.z),(self.size.x,-self.size.y,self.size.z),(self.size.x,self.size.y,self.size.z))]
			self.polygon = QtGui.QPolygonF()
                	for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)/100,(t.y-self.scene.bottom)/100))
                	self.i.setPolygon(self.polygon)
class eventSensorReturnsFactory(SensorReturnsFactory):
	def __init__(self,scene):
		SensorReturnsFactory.__init__(self)
		self.scene = scene
	def create(self,u):
		e=eventSensorReturns(u,self.scene,self.mapmodel)
		debug("Created eventSensorReturns "+`e`)
		e.__disown__()
		debug("Disowned eventSensorReturns "+`e`)
		p=UpdatedSensorReturns_CreateUpdatedSensorReturnsPtr(e)
		debug("Returning pointer "+`p`)
		p.thisown=0 #TODO: check, as in eventUnitFactory above
		return p
class eventSensorReturns(UpdatedSensorReturns):
	def __init__(self,u,scene,m):
		debug("Creating eventSensorReturns")
		UpdatedSensorReturns.__init__(self,u)
		self.scene = scene
		self.m = m
		self.i = None
		self.altered()
	def altered(self):
		debug("About to get perception")
		p = self.getPerception()
		if (p & sakusen.perception_owner):
			debug("Able to perceive owner")
		if (p & sakusen.perception_region):
			debug("Able to perceive approximate region")
		if (p & sakusen.perception_unit):
			debug("Able to perceive full unit")
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
			for t in corners: self.polygon.append(QtCore.QPointF((t.x-self.scene.left)/100,(t.y-self.scene.bottom)/100))
			if(not self.i):
				self.i=QtGui.QGraphicsPolygonItem(self.polygon,self.m.i)
				self.i.setPen(QtGui.QPen(QtGui.QColor('red')))
			else:
				self.i.setPolygon(self.polygon)
class gameModel(QtCore.QObject):
	def __init__(self,clientid):
		QtCore.QObject.__init__(self)
		self.clientid=clientid
		#operator/ is a bitch to wrap, so just do everything with /= - it only uses that internally anyway
		d=fileUtils_getHome()
		d/=path(CONFIG_SUBDIR)
		d/=path(DATA_SUBDIR)
		self.resourceinterface=FileResourceInterface_create(d,False)
		self.scene = 0 #arrgh, need to fix this
	def setUniverse(self,universe): #TODO: check hash
		debug("Universe name changed, setting")
		result=self.resourceinterface.searchUniverse(universe)
		if(g(result,1)!=resourceSearchResult_success):
			print "Failed finding universe \"%s\", error %d"%(universe,g(result,1))
			raise Exception()
		debug("If the universe doesn't get set now, something fishy's going on")
		self.universe=g(result,0)
		debug("Set universe to "+`self.universe`)
	def createWorld(self,d):
		pass
		#self.w=PartialWorld(self.universe,e,sf,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
		#w should now get deleted at the correct time, I think
		#debug("Created partial world %s"%self.w)
		#debug("Global variable world is now %s"%`sakusen_client.cvar.world`)
	def pushUpdates(self,d):
		while(d.getTime()>sakusen_client.cvar.world.getTimeNow()):
			sakusen_client.cvar.world.endTick()
		if(d.getTime()>sakusen_client.cvar.world.getTimeNow()):
			print "Got updates in wrong order"
		l=d.getUpdates()
		for u in l:
			debug("pushing update "+`u`)
			sakusen_client.cvar.world.applyUpdate(u)
			debug("Applied an update")
	def setScene(self,scene):
		self.scene = scene
