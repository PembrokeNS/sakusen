#!/usr/bin/env python
from mainWindowImpl import orders
from singleton import interestingthings

orders = {}
interestingthings['orders'] = orders
availableOrders = {}
interestingthings['availableOrders'] = availableOrders

class KiaiOrder:
    def __init__(self, name):
        availableOrders[name] = self

class SakusenOrder(KiaiOrder):
	def __init__(self):
		super.__init__(self, 'sakusenOrder')
	def __call__(self, od):
		od.thisown = 0
		o = Order(od)
	    om = OrderMessage(kwargs['unit'].getId(), o)
		omd = OrderMessageData(om)
	    interestingthings['socket'].sendd(omd)

SakusenOrder()
