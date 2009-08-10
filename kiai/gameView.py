#!/usr/bin/env python
from PyQt4 import QtGui

class gameView(QtGui.QGraphicsView):
	def __init__(self, parent):
		QtGui.QGraphicsView.__init__(self,parent)
		self.setTransformationAnchor(self.AnchorUnderMouse)
	def wheelEvent(self, e):
		e.accept()
		self.scale(1.001**e.delta(),1.001**e.delta())
