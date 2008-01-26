#!/usr/bin/env python
from PyQt4 import QtCore,QtGui
class sceneModel(QtCore.QObject):
	def __init__(self):
		self.s=QtGui.QGraphicsScene()
	def createUnit(self,typedata,status):
		pass