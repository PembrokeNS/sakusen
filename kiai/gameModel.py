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
		utypedata=self.getTypeData()
		ustatus=self.getStatus()
		self.emit(QtCore.SIGNAL("unitCreated(PyQt_PyQbject,PyQt_PyQbject)"),utypedata,ustatus)
		self.rect = u.getBoundingRectangle()
		self.i=QtGui.QGraphicsRectItem((self.rect.minx-scene.left)/100.0,(self.rect.miny-scene.bottom)/100.0,(self.rect.maxx-self.rect.minx)/100.0,(self.rect.maxy-self.rect.miny)/100.0,m.i)
		debug("Created eventUpdatedUnit")
class eventSensorReturnsFactory(SensorReturnsFactory):
	def __init__(self,scene):
		SensorReturnsFactory.__init__(self)
		self.scene = scene
	def create(self,u):
		e=eventSensorReturns(u,self.scene,self.mapmodel)
		debug("Created eventSensorReturns "+`e`)
		e.thisown=0 # will probably need to be e.__disown__() once eventSensorReturns gets any code
		debug("Disowned eventSensorReturns "+`e`)
		p=UpdatedSensorReturns_CreateUpdatedSensorReturnsPtr(e)
		debug("Returning pointer "+`p`)
		p.thisown=0 #TODO: check, as in eventUnitFactory above
		return p
class eventSensorReturns(UpdatedSensorReturns):
	def __init__(self,u,scene,m):
		debug("Creating eventSensorReturns")
		UpdatedSensorReturns.__init__(self,u)
		p = self.getPerception()
		if (p & sakusen.perception_owner):
			debug("Able to perceive owner")
		if (p & sakusen.perception_region):
			debug("Able to perceive approximate region")
		if (p & sakusen.perception_unit):
			debug("Able to perceive full unit")
			r = self.getUnit() #reference to the unit. Isn't properly wrapped, but fortunately all we do with it for now is:
			#TODO: fix deletion of r
			self.u = sakusen.CompleteUnit(r) #the unit
			self.u.thisown=0
			self.rect = u.getBoundingRectangle() #bounding rectangle
			self.rect.thisown=0
			self.i=QtGui.QGraphicsRectItem((self.rect.minx-scene.left)/100.0,(self.rect.miny-scene.bottom)/100.0,(self.rect.maxx-self.rect.minx)/100.0,(self.rect.maxy-self.rect.miny)/100.0,m.i)
			self.i.setPen(QtGui.QPen(QtGui.QColor('red')))
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
