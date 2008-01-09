#!/usr/bin/env python
from sakusen import *
from sakusenresources import *
from PyQt4 import QtCore
def debug(x): print x
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
			raise Exception()