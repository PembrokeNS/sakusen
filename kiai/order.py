#!/usr/bin/env python
from mainWindowImpl import orders
from singleton import interestingthings

availableOrders = set()
interestingthings['availableOrders'] = availableOrders

class Order:
    def __init__(self):
        orders.append(self)
    def addToUi(self):
        self.mainwindow.ui.orders.layout().addWidget(self.button)
