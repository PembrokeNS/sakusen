#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys

from PyQt4 import QtCore
from PyKDE4 import kdecore,kdeui

from sakusen import *
from sakusen_resources import *
from sakusen_comms import *

from connectDialogImpl import connectDialog
from mainWindowImpl import mainWindow
from socketModel import socketModel


KIAI_SUBDIR="kiai"
interestingthings = {} #the things we want user scripting to be able to interact with

def userconfig(s):
	"""Try and execute the specified user configuration file"""
	configpath=fileUtils_getHome()
	configpath/=path(CONFIG_SUBDIR)
	configpath/=path(KIAI_SUBDIR)
	configpath/=path(s+".py")
	try:
		execfile(configpath.string(),interestingthings)
	except IOError:
		print "No configuration file \"%s\" found, or failed to open that file"%(configpath.string())

userconfig("startup")
aboutdata = kdecore.KAboutData("kiai","",kdecore.ki18n("Kiai"),"0.0.4 気持ち",kdecore.ki18n("Sakusen client"),kdecore.KAboutData.License_Custom,kdecore.ki18n("(c) 2007-9 IEG/lmm"),kdecore.ki18n("none"),"none","md401@srcf.ucam.org") # necessary to keep a reference to this around, otherwise it gets GCed at the wrong time and we segfault. Call that a pykde bug.
kdecore.KCmdLineArgs.init(sys.argv, aboutdata)
sys.argv = [""] # cProfile wants to modify it, even though kde already has.
a=kdeui.KApplication()
Socket_socketsInit()

d=fileUtils_getHome()
d/=path(CONFIG_SUBDIR)
d/=path(DATA_SUBDIR)
resourceinterface=FileResourceInterface_create(d,False)

mainwindow = mainWindow(interestingthings)
w=connectDialog()
mainwindow.show()
activeSocket = socketModel(interestingthings, mainwindow, resourceinterface, userconfig)
interestingthings['socket'] = activeSocket
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),activeSocket.join)
QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),activeSocket.leave)
w.show()
#sys.stdout = mainwindow
#sys.stderr = mainwindow
r=a.exec_()
