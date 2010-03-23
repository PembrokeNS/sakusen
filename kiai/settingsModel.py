#!/usr/bin/env python

from PyQt4 import QtCore

import string

from sakusen_comms import *

class settingItem:
	#TODO: we could store whether a node is a leaf, and thus provide an efficient implementation of settingsModel.hasChildren()
	def __init__(self,path,selfindex,parent,data,column=0):
		self.path=path
		self.selfindex=selfindex
		self.parent=parent
		if(data):
			self.data=data
		else:
			if(column==0):
				self.data=path[-1]
			else:
				self.data='[unset]'
		self.children=()
		self.column=column
	def rowCount(self): return len(self.children)
	def index(self,row,column):
		if(row<0 or row>=len(self.children) or column <0 or column >1): return QtCore.QModelIndex()
		else:
			path=self.path+(self.children[row],)
			try:
				r=self.selfindex.model().indexof(path,column)
				return self.selfindex.model().createIndex(row,column,r)
			except Exception:
				if(column==1): return QtCore.QModelIndex()
				self.selfindex.model().l.append(settingItem(path,None,self.selfindex,None))
				i=self.selfindex.model().createIndex(row,column,self.selfindex.model().indexof(path,column))
				self.selfindex.model().l[i.internalId()].selfindex=i
				self.selfindex.model().requestSetting(path)
				return i

class settingsModel(QtCore.QAbstractItemModel):
	def __init__(self, socket, parent=None):
		QtCore.QAbstractItemModel.__init__(self,parent)
		self.socket = socket
		self.l=[settingItem(('',),self.createIndex(0,0,0),QtCore.QModelIndex(),"[root]")]
	def data(self,index,role):
		if((role!=QtCore.Qt.DisplayRole) or not index.isValid()): return None
		else: return self.l[index.internalId()].data
	def parent(self,index):
		if(not index.isValid()): return QtCore.QModelIndex()
		else:
			return self.l[index.internalId()].parent
	def rowCount(self,index):
		if(not index.isValid()): return 1
		else: return self.l[index.internalId()].rowCount()
	def columnCount(self,index): return 2
	def index(self,row,column,index):
		if(not index.isValid()):
			if(row==0 and column==0):
				return self.l[0].selfindex #creating index again might be a bad idea
			else:
				return QtCore.QModelIndex()
		else:
			return self.l[index.internalId()].index(row,column)
	def indexof(self,path,column):
		for i in range(len(self.l)):
			if(self.l[i].path==path and self.l[i].column==column): return i
		raise Exception()
	def headerData(self,section,orientation,role):
		if(role==QtCore.Qt.DisplayRole and orientation==QtCore.Qt.Horizontal and 0<=section and 2>=section):
			if(section==0):
				return "Node"
			else:
				return "Data"
		else:
			return None
	def processUpdate(self,d):
		self.emit(QtCore.SIGNAL("layoutAboutToBeChanged()"))
		path=tuple(d.getSetting().split(':'))
		try:
			n=self.indexof(path,0)
		except Exception:
			#update was to a setting we're not displaying - probably happened because user manually set one with setSetting before the tree was expanded
			return
		if(not d.isLeaf()):
			#this is ugly, but I don't know any way to be sure the set of children hasn't changed - and it certainly does sometimes. Thankfully the Qt class is designed to handle this sort of thing - but it leaves a lot of exceptions on the console.
			self.beginRemoveRows(self.l[n].selfindex,0,len(self.l[n].children)-1)
			self.l[n].children=[]
			self.endRemoveRows()
			self.beginInsertRows(self.l[self.indexof(path,0)].selfindex,0,len(d.getValue())-1)
			self.l[n].children=d.getValue()
			self.endInsertRows()
		else:
			try:
				m=self.indexof(path,1)
				self.l[m].data=d.getValue()[0]
				self.emit(QtCore.SIGNAL("dataChanged(const QModelIndex &,const QModelIndex &)"),self.l[m].selfindex,self.l[m].selfindex)
			except Exception:
				self.beginInsertColumns(self.l[self.indexof(path[:-1],0)].selfindex,1,1)
				self.l.append(settingItem(path,None,self.l[n].parent,(d.getValue() or ('',))[0],1))
				i=self.indexof(path,1)
				self.l[i].selfindex=self.createIndex(self.l[n].selfindex.row(),1,i)
				self.endInsertColumns()
		self.emit(QtCore.SIGNAL("layoutChanged()"))
	def flags(self,index):
		if(index.isValid() and index.column()==1): return QtCore.Qt.ItemIsEditable|QtCore.Qt.ItemIsEnabled|QtCore.Qt.ItemIsSelectable
		else: return QtCore.Qt.ItemIsEnabled|QtCore.Qt.ItemIsSelectable
	def setData(self,index,value,role):
		if(index.isValid()): 
			self.setSetting(self.l[index.internalId()].path,str(value.toString()))
			return True
		else:
			return False
	def requestSetting(self, path):
		s = ':'.join(path)
		d = GetSettingMessageData(s)
		self.socket.sendd(d)
	def setSetting(self, path, value):
		s = ':'.join(path)
		d = ChangeSettingMessageData(s,value)
		self.socket.sendd(d)
