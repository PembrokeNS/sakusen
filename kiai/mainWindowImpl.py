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
		self.connect(self.ui.move, QtCore.SIGNAL('toggled(bool)'), self.setmove)
		self.connect(self.ui.attack, QtCore.SIGNAL('toggled(bool)'), self.setattack)
		self.connect(self.ui.build, QtCore.SIGNAL('toggled(bool)'), self.setbuild)
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
	def setmove(self, t):
		if(t):
			self.ui.gameview.scene().mpe = move
			self.othercommands(self.ui.move)
	def setattack(self, t):
		if(t):
			self.ui.gameview.scene().mpe = attack
			self.othercommands(self.ui.attack)
	def setbuild(self, t):
		if(t):
			self.ui.gameview.scene().mpe = build
			self.othercommands(self.ui.build)
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

@mpeFunction
def move(**kwargs):
	for i in kwargs['selectedUnits']:
		u = i.unit
		od = MoveOrderData(kwargs['targetPoint'])
		od.thisown = 0
		o = Order(od)
		om = OrderMessage(u.getId(), o)
		omd = OrderMessageData(om)
		kwargs['socket'].sendd(omd)

@mpeFunction
def attack(**kwargs):
	for i in kwargs['selectedUnits']:
		u = i.unit
		s = u.getStatus()
		utype = s.getTypePtr()
		for j,w in enumerate(utype.getWeapons()):
			if(w.getClientHint() == 'o'):
				try:
					od = TargetSensorReturnsOrderData(j, kwargs['targetObject'].sr.getRefToThis())
				except AttributeError:
					od = TargetPositionOrderData(j, kwargs['targetPoint'])
				od.thisown = 0 
				o = Order(od)
				om = OrderMessage(u.getId(), o)
				omd = OrderMessageData(om)
				kwargs['socket'].sendd(omd)

@mpeFunction
def build(**kwargs):
	for i in kwargs['selectedUnits']:
		u = i.unit
		s = u.getStatus()
		utype = s.getTypePtr()
		for j,w in enumerate(utype.getWeapons()):
			if(w.getClientHint()[:2] == 'b'):
				try:
					od = TargetUnitOrderData(j, kwargs['targetObject'].unit.getRefToThis())
					od.thisown = 0
					o = Order(od)
					om = OrderMessage(u.getId(), o)
					omd = OrderMessageData(om)
					kwargs['socket'].sendd(omd)
				except AttributeError: pass





