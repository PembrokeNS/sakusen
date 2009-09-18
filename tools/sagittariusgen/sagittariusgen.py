#!/usr/bin/env python

from sakusen import *
from sakusen_resources import *

if(__name__=="__main__"):
	spath = fileUtils_getHome()
	spath /= path(CONFIG_SUBDIR)
	spath /= path(DATA_SUBDIR)
	resint = FileResourceInterface_create(spath, False)
	u = Universe("sagittarius", "", "sagittarius", "create_script", [], [])
	err = u.resolveNames()
	print("Resolving names: %s"%err)
	u.thisown = 0 #because shared_ptr
	up = Universe.createConstPtr(u)
	res = resint.usave(up, "sagittarius")
	print("Tried to save, result: %d" % res)
