#!/usr/bin/env python
from PyQt4 import QtGui,QtCore
from PyKDE4 import kdeui,kdecore
from sakusen import *
def debug(x): pass

class mapModel:
	def __init__(self,m):
		#todo: lose this lame granularity
		w=m.width()/100
		h=m.height()/100
		debug("Creating map image, width %s, height %s"%(`w`,`h`))
		i=QtGui.QImage(w,h,QtGui.QImage.Format_RGB32) #use QSize because otherwise sip gets confused about the overloads, I assume. Certainly craziness occurs - created image is 0x0
		debug("Created image, width %d, height %d"%(i.width(),i.height()))
		h=m.getHeightfield()
		l=m.left()
		b=m.bottom()
		debug("Looping from %d to %d"%(m.left(),m.right()))
		d=QtGui.QProgressDialog("Rendering map...","This button should not exist, but PyQt insists on it",0,w)
		for x in range(m.left(),m.right(),100):
			d.setValue((x-l)/100)
			for y in range(m.bottom(),m.top(),100):
				c=h.getApproxHeightAt(SPoint32(x,y,0)) #TODO: use getApproxHeightAt - probably requires casting, ololol
				assert(c<2**16 and c>-2**16)
				c+=2**16
				c/=2**9
				i.setPixel((x-l)/100,(y-b)/100,QtGui.QColor(c,c,c).rgb())
			kdeui.KApplication.kApplication().processEvents()
		d.setValue(w)
		p=QtGui.QPixmap.fromImage(i)
		self.i=QtGui.QGraphicsPixmapItem(p)
