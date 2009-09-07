#!/usr/bin/env python
from PyQt4 import QtCore
def debug(x): pass
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
				self.selfindex.model().emit(QtCore.SIGNAL("requestSetting(PyQt_PyObject)"),path)
				return i

class settingsModel(QtCore.QAbstractItemModel):
	def __init__(self,parent=None):
		debug("creating settingsModel")
		QtCore.QAbstractItemModel.__init__(self,parent)
		self.l=[settingItem(('',),self.createIndex(0,0,0),QtCore.QModelIndex(),"[root]")]
		debug("created settingsModel")
	def data(self,index,role):
		if((role!=QtCore.Qt.DisplayRole) or not index.isValid()): return QtCore.QVariant()
		else: return QtCore.QVariant(self.l[index.internalId()].data)
	def parent(self,index):
		if(not index.isValid()): return QtCore.QModelIndex()
		else:
			return self.l[index.internalId()].parent
	def rowCount(self,index):
		if(not index.isValid()): return 1
		else: return self.l[index.internalId()].rowCount()
	def columnCount(self,index): return 2
	def index(self,row,column,index):
		if(index<=0 or not index.isValid()): #remove the index<=0 once it's understood
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
				return QtCore.QVariant("Node")
			else:
				return QtCore.QVariant("Data")
		else:
			return QtCore.QVariant()
	def processUpdate(self,d):
		self.emit(QtCore.SIGNAL("layoutAboutToBeChanged()"))
		path=tuple(d.getSetting().split(':'))
		debug("working on path "+`path`)
		try:
			n=self.indexof(path,0)
		except Exception:
			print "Got unexpected update to setting "+`path`+", ignoring"
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
				debug("about to update setting "+`path`+" with value "+d.getValue()[0])
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
			self.emit(QtCore.SIGNAL("editSetting(PyQt_PyObject,PyQt_PyObject)"),self.l[index.internalId()].path,str(value.toString()))
			return True
		else:
			return False

