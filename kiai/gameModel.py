#!/usr/bin/env python
from sakusen import *
from sakusenresources import *
from sakusenclient import *
import sakusenclient
from PyQt4 import QtCore
def debug(x): print x

class eventUnitFactory(UnitFactory):
	def create(self,u):
		debug("Creating eventUpdatedUnit")
		return eventUpdatedUnit(u)
class eventUpdatedUnit(UpdatedUnit):
	pass
class gameModel(QtCore.QObject):
	def __init__(self,clientid):
		QtCore.QObject.__init__(self)
		self.clientid=clientid
		#operator/ is a bitch to wrap, so just do everything with /= - it only uses that internally anyway
		d=fileUtils_getHome()
		d/=path(CONFIG_SUBDIR)
		d/=path(DATA_SUBDIR)
		self.resourceinterface=FileResourceInterface_create(d,False)
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
		#TODO: currently, better hope the player looked up the universe beforehand
		debug("Game started, creating world")
		e=eventUnitFactory()
		w=PartialWorld(self.universe,e,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
		w.thisown=False #we can't keep w, it's got to make it on its own; it'd go out of scope and die very fast if we left it here. It'll be deleted when the universe gets destroyed, I think
		e.thisown=0 #w takes ownership of e
		debug("Created partial world %s"%w)
		debug("Global variable world is now %s"%`sakusenclient.cvar.world`)
	def pushUpdates(self,d):
		debug("Received updates to game state")
		while(d.getTime()>sakusenclient.cvar.world.getTimeNow()):
			sakusenclient.cvar.world.endTick()
		if(d.getTime()>sakusenclient.cvar.world.getTimeNow()):
			print "Got updates in wrong order"
		l=d.getUpdates()
		debug("Update list is "+`l`)
		for u in l:
			sakusenclient.cvar.world.applyUpdate(u)
		debug("Updates successfully applied")