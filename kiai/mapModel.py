#!/usr/bin/env python
from PyQt4 import QtGui,QtCore
from sakusen import *
def debug(x): pass

class mapModel():
	def __init__(self,m):
		w=m.width()
		h=m.height()
		debug("Creating map image, width %s, height %s"%(`w`,`h`))
		i=QtGui.QImage(w,h,QtGui.QImage.Format_RGB32) #use QSize because otherwise sip gets confused about the overloads, I assume. Certainly craziness occurs - created image is 0x0
		debug("Created image, width %d, height %d"%(i.width(),i.height()))
		h=m.getHeightfield()
		l=m.left()
		b=m.bottom()
		debug("Looping from %d to %d"%(m.left(),m.right()))
		for x in range(m.left(),m.right()):
			for y in range(m.bottom(),m.top()):
				c=h.getHeightAt(SPoint32(x,y,0)) #TODO: use getApproxHeightAt - probably requires casting, ololol
				assert(c<2**16 and c>-2**16)
				c+=2**16
				c/=2**9
				i.setPixel(x-l,y-b,QtGui.QColor(c,c,c).rgb())
		p=QtGui.QPixmap.fromImage(i)
		self.i=QtGui.QGraphicsPixmapItem(p)
