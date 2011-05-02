#!/usr/bin/env python
from singleton import interestingthings

orders = {}
interestingthings['orders'] = orders

class KiaiOrder:
	def __init__(self, name):
		orders[name] = self

class SakusenOrder(KiaiOrder):
	def __init__(self):
		KiaiOrder.__init__(self, 'sakusenOrder')
	def __call__(self, od):
		od.thisown = 0
		o = Order(od)
		om = OrderMessage(kwargs['unit'].getId(), o)
		omd = OrderMessageData(om)
		interestingthings['socket'].sendd(omd)

#SakusenOrder()
