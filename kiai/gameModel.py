#!/usr/bin/env python
# -*- coding: utf-8 -*-
from sakusen import *
from sakusen_resources import *
from sakusen_client import *
import sakusen_client
from PyQt4 import QtCore,QtGui
import sip

class gameModel(QtCore.QObject):
	def __init__(self):
		QtCore.QObject.__init__(self)
		#operator/ is a bitch to wrap, so just do everything with /= - it only uses that internally anyway
		d=fileUtils_getHome()
		d/=path(CONFIG_SUBDIR)
		d/=path(DATA_SUBDIR)
		self.resourceinterface=FileResourceInterface_create(d,False)
		self.scene = 0 #arrgh, need to fix this
	def setUniverse(self,universe): #TODO: check hash
		result=self.resourceinterface.searchUniverse(universe)
		if(g(result,1)!=resourceSearchResult_success):
			print "Failed finding universe \"%s\", error %d"%(universe,g(result,1))
			raise Exception()
		self.universe=g(result,0)
	def pushUpdates(self,d):
		while(d.getTime()>sakusen_client.cvar.world.getTimeNow()):
			sakusen_client.cvar.world.endTick()
		if(d.getTime()>sakusen_client.cvar.world.getTimeNow()):
			print "Got updates in wrong order"
		l=d.getUpdates()
		for u in l:
			sakusen_client.cvar.world.applyUpdate(u)
	def setScene(self,scene):
		self.scene = scene
