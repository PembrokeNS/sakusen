#!/usr/bin/env python
from sakusen import *
from sakusenresources import *
from sakusenclient import *
import sakusenclient
from PyQt4 import QtCore
import imp
def debug(x): pass

class eventUnitFactory(UnitFactory):
	def create(self,u):
		e=eventUpdatedUnit(u)
		debug("Creating eventUpdatedUnit "+`e`)
		e.__disown__() #the C++ code will look after it. TODO: see if return eventUpdatedUnit().__disown__() is a nicer ideom
		p=UpdatedUnit_CreateUpdatedUnitPtr(e)
		debug("Returning pointer "+`p`)
		p.thisown=0 #I don't even know any more - TODO: CHECK THIS
		return p
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
		e.__disown__() #w takes ownership of e; we have to do this differently because it is a director class, *sigh*
		self.w=PartialWorld(self.universe,e,d.getPlayerId(),d.getTopology(),d.getTopRight(),d.getBottomLeft(),d.getGravity(),d.getHeightfield())
		#w should now get deleted at the correct time, I think
		debug("Created partial world %s"%self.w)
		debug("Global variable world is now %s"%`sakusenclient.cvar.world`)
	def pushUpdates(self,d):
		while(d.getTime()>sakusenclient.cvar.world.getTimeNow()):
			sakusenclient.cvar.world.endTick()
		if(d.getTime()>sakusenclient.cvar.world.getTimeNow()):
			print "Got updates in wrong order"
		l=d.getUpdates()
		for u in l:
			debug("pushing update "+`u`)
			sakusenclient.cvar.world.applyUpdate(u)
			debug("Applied an update")
		debug("Updates successfully applied")