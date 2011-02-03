#!/usr/bin/env python
from mainWindowImpl import orders

class Order:
    def __init__(self):
        orders.append(self)
    def addToUi(self):
        self.mainwindow.ui.orders.layout().addWidget(self.button)
