#!/usr/bin/env python
from PyQt4 import QtGui

def color(curhp, maxhp):
	r = (curhp * 511) // maxhp
	if(r > 256):
		return QtGui.QColor(511 - r, 255, 0)
	else:
		return QtGui.QColor(255, r, 0)
	
