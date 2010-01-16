#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyKDE4 import kdeui
from PyQt4 import QtCore

from sakusen import *
from sakusen_comms import *

from mainWindow import Ui_mainWindow

from sceneModel import mpeFunction, sceneModel

class mainWindow(kdeui.KXmlGuiWindow):
	def __init__(self, environment, parent=None):
		kdeui.KXmlGuiWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)
		self.setupGUI()
		self.connect(self.ui.entryline, QtCore.SIGNAL('returnPressed(const QString &)'), self.runcommand)
		self.connectUp(self.ui.move, move)
		self.connectUp(self.ui.attack, attack)
		self.connectUp(self.ui.build, build)
		self.connect(self.ui.stop, QtCore.SIGNAL('clicked()'), self.stop)
		self.env = environment
	def runcommand(self, t):
		self.ui.output.append(">"+t)
		self.ui.entryline.setText("")
		exec str(t) in self.env
	def write(self, t):
		self.ui.output.append(t.strip())
	def othercommands(self, button):
		button.setEnabled(False)
		for b in self.findChildren(kdeui.KPushButton):
			if(b != button):
				b.setEnabled(True)
				b.setChecked(False)
	def connectUp(self, button, function):
		def setf(t):
			if(t):
				self.ui.gameview.scene().mpe = function
				self.othercommands(button)
		QtCore.QObject.connect(button, QtCore.SIGNAL('toggled(bool)'), setf)
	def stop(self):
		self.ui.gameview.scene().mpe = sceneModel.mpe
		for i in self.ui.gameview.scene().selectedItems():
			u = i.unit
			s = u.getStatus()
			utype = s.getTypePtr()
			for j,w in enumerate(utype.getWeapons()):
				if(w.getClientHint() == 'o'):
					od = TargetNoneOrderData(j);
					od.thisown = 0 
					o = Order(od)
					om = OrderMessage(u.getId(), o)
					omd = OrderMessageData(om)
					self.ui.gameview.scene().sock.sendd(omd)
		self.othercommands(self.ui.stop)
		self.ui.stop.setEnabled(True)

def orderSelectedUnits(f):
	def g(**kwargs):
		for i in kwargs['selectedUnits']:
			kwargs['unit'] = i.unit
			od = f(**kwargs)
			od.thisown = 0
			o = Order(od)
			om = OrderMessage(kwargs['unit'].getId(), o)
			omd = OrderMessageData(om)
			kwargs['socket'].sendd(omd)
	return g

def forWeapons(hint):
	def g(f, **kwargs):
		s = kwargs['unit'].getStatus()
		utype = s.getTypePtr()
		for j,w in enumerate(utype.getWeapons()):
			if(w.getClientHint().startswith(hint)):
				kwargs['weaponId'] = j
				return f(**kwargs)
	return lambda f: lambda **kwargs: g(f, **kwargs)

@mpeFunction
@orderSelectedUnits
def move(**kwargs):	return MoveOrderData(kwargs['targetPoint'])

@mpeFunction
@orderSelectedUnits
@forWeapons('o')
def attack(**kwargs):
	try: return TargetSensorReturnsOrderData(kwargs['weaponId'], kwargs['targetObject'].sr.getRefToThis())
	except AttributeError: return TargetPositionOrderData(kwargs['weaponId'], kwargs['targetPoint'])

@mpeFunction
@orderSelectedUnits
@forWeapons('b')
def build(**kwargs):
	try: return TargetUnitOrderData(kwargs['weaponId'], kwargs['targetObject'].unit.getRefToThis())
	except AttributeError: pass
