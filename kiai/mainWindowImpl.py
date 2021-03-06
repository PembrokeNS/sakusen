#!/usr/bin/env python
# -*- coding: utf-8 -*-
from PyKDE4 import kdeui
from PyQt4 import QtCore

from sakusen import *
from sakusen_comms import *

from mainWindow import Ui_mainWindow

from sceneModel import mpeFunction, orderSelectedUnits, forWeapons, sceneModel
from orientationDialogImpl import OrientationDialog
from singleton import interestingthings
from order import orders

class mainWindow(kdeui.KXmlGuiWindow):
	def __init__(self, parent=None):
		kdeui.KXmlGuiWindow.__init__(self,parent)
		self.ui=Ui_mainWindow()
		self.ui.setupUi(self)
		self.setupGUI()
		self.connect(self.ui.entryline, QtCore.SIGNAL('returnPressed(const QString &)'), self.runcommand)
		self.connectUp(self.ui.move, move)
		self.connectUp(self.ui.attack, attack)
		self.connectUp(self.ui.build, build)
		orders['stop'] = self.stop
		orders['orient'] = self.orient
		#For debugging only, remove when extension API is finalized (hahahahaha)
		interestingthings['mainwindow'] = self
	def runcommand(self, t):
		self.ui.output.append(">"+t)
		self.ui.entryline.setText("")
		exec(str(t), interestingthings)
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
		self.stopcmd(selectedUnits = self.ui.gameview.scene().selectedItems(), socket = self.ui.gameview.scene().sock)

	def orient(self):
		d = OrientationDialog()
		self.connect(d, QtCore.SIGNAL('valueChanged(double)'), self.orientto)
		d.show()
		self.orientationDialog = d

	def orientto(self, orientation):
		self.orientorder(orientation, selectedUnits = self.ui.gameview.scene().selectedItems(), socket = self.ui.gameview.scene().sock)

	@staticmethod
	@orderSelectedUnits
	def orientorder(orientation, **kwargs):
		orientation = (270 - orientation) % 360 #TODO: abstract this out
		angle = int(orientation * 100)
		rotation = rotation_anticlockwise
		orientation = Orientation(rotation, angle)
		return OrientOrderData(orientation)

	@staticmethod
	@orderSelectedUnits
	@forWeapons('o')
	def stopcmd(**kwargs):
		return TargetNoneOrderData(kwargs['weaponId'])

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
