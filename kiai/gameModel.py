#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sakusen import *
from sakusen_resources import *
from sakusen_client import *
import sakusen_client
from PyQt4 import QtCore,QtGui
import sip

def debug(x): pass

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
