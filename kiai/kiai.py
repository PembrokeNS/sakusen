#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import division

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
	configpath = fileUtils_getHome() / path(SAKUSEN_CONFIG_SUBDIR) / path(KIAI_SUBDIR) / path(s+".py")
	try:
		exec(open(configpath.string(),'r').read(),interestingthings)
	except IOError:
		print("No configuration file \"%s\" found, or failed to open that file"%(configpath.string()))

userconfig("startup")
# necessary to keep a reference to this around, otherwise it gets GCed at the wrong time and we segfault. Call that a pykde bug.
aboutdata = kdecore.KAboutData(b"kiai", b"", kdecore.ki18n(b"Kiai"), u"0.0.4 気持ち".encode('utf-8'), kdecore.ki18n(b"Sakusen client"), kdecore.KAboutData.License_Custom, kdecore.ki18n(b"(c) 2007-10 IEG/lmm"), kdecore.ki18n(b"none"), b"none", b"md401@srcf.ucam.org")
kdecore.KCmdLineArgs.init(sys.argv, aboutdata)
sys.argv = [""] # cProfile wants to modify it, even though kde already has.
a=kdeui.KApplication()
Socket_socketsInit()

d=fileUtils_getHome()
d/=path(SAKUSEN_CONFIG_SUBDIR)
d/=path(SAKUSEN_RESOURCES_SUBDIR)
resourceinterface=FileResourceInterface_create(d,False)

mainwindow = mainWindow(interestingthings)
w=connectDialog()
mainwindow.show()
activeSocket = socketModel(interestingthings, mainwindow, resourceinterface, userconfig)
interestingthings['socket'] = activeSocket
QtCore.QObject.connect(w,QtCore.SIGNAL("openConnection(QString)"),activeSocket.join)
QtCore.QObject.connect(a,QtCore.SIGNAL("aboutToQuit()"),activeSocket.leave)
w.show()
sys.stdout = mainwindow
sys.stderr = mainwindow
r=a.exec_()
