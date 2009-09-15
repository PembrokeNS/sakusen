#!/usr/bin/env python

from PyQt4 import QtGui,QtCore
from PyKDE4 import kdeui,kdecore
from sakusen import *

class mapModel:
	"""Represents the map - mainly its width, height, heightfield and a pixmap of the heights"""
	def __init__(self,m):
		#TODO: lose this lame granularity
		w=m.width()/100
		h_=m.height()/100
		i=QtGui.QImage(w,h_,QtGui.QImage.Format_RGB32)
		self.h=m.getPartialHeightfield()
		hwidth = self.h.getWidth()
		hheight = self.h.getHeight()
		hdata = self.h.getHeightfieldPtr()
		hres = self.h.getXYResolution()
		hzres = self.h.getZResolution()
		#himage = QtGui.QImage(hdata,hwidth,hheight,QtGui.QImage.Format_RGB16) #This is the function to use, when it's properly wrapped
		j = QtGui.QImage(hwidth, hheight, QtGui.QImage.Format_RGB32)
		for x in range(hwidth):
			for y in range(hheight):
				c=(hdata[x*hheight + y] * hzres + 2**16) / 2**9
				j.setPixel(x,y,QtGui.QColor(c,c,c).rgb())
		k = j.scaled((hwidth*hres)/100,(hheight*hres)/100)
		l = k.copy(0,0,w,h_)
		p=QtGui.QPixmap.fromImage(l)
		self.i=QtGui.QGraphicsPixmapItem(p)
		self.i.setAcceptedMouseButtons(QtCore.Qt.NoButton) #map doesn't accept any kind of mouse press.
